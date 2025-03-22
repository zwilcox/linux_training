#include <linux/spinlock.h>
#include <linux/mm_types.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/major.h>
#include <linux/poll.h>
#include <linux/cred.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/page.h>

#include "mapshared.h"

static int map_open(struct inode* node, struct file* filp);
static int map_release(struct inode* node, struct file* filp);
static int map_mmap(struct file*, struct vm_area_struct*);
static __poll_t map_poll(struct file*, struct poll_table_struct*);

struct file_operations mapops = {
    .owner = THIS_MODULE,
    .open = map_open,
    .release = map_release,
    .mmap = map_mmap,
    .poll = map_poll,
};

struct map_device { //used to store device info
    struct cdev cdev;
    char* maparea;
	int nextoff;
	spinlock_t lk;
	wait_queue_head_t queue;
};

struct map_per_file { //used to store reference to our device
	struct map_device* dev;
	int lastoff;
};

struct map_device mdev;

static int __init mod_start(void) {
    int err = 0;
    printk(KERN_ALERT "Starting map module\n");

    err = register_chrdev_region(MKDEV(MAP_MAJ, MAP_MIN), 1, "map_device_driver");
    if (err != 0) {
        printk(KERN_ALERT "Got error while registering region (%d,%d)\n", MAP_MAJ, MAP_MIN);
        return err;
    }

	spin_lock_init(&mdev.lk); 
	init_waitqueue_head(&mdev.queue);
	mdev.nextoff = 0;
    mdev.maparea = (char*)get_zeroed_page(GFP_KERNEL); //could also use kmalloc or kmalloc

    cdev_init(&(mdev.cdev), &mapops);
    err = cdev_add(&(mdev.cdev), MKDEV(MAP_MAJ, MAP_MIN), 1);
	if (err != 0) {
		printk(KERN_ALERT "Got error while adding cdev\n");
		return err;
	}

    printk(KERN_ALERT "Setup device %d:%d\n", MAP_MAJ, MAP_MIN);

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "map module Exiting\n");
	
	unregister_chrdev_region(MKDEV(MAP_MAJ, MAP_MIN), 1);

    cdev_del(&(mdev.cdev));

    free_pages((unsigned long)mdev.maparea, 0);

	return;
}

static int map_open(struct inode* node, struct file* filp) {
    struct map_device* dev;
	struct map_per_file* mpf;

    printk(KERN_ALERT "Open got called\n");

    dev = container_of(node->i_cdev, struct map_device, cdev);

	mpf = kmalloc(sizeof(struct map_per_file), GFP_KERNEL);
	if (mpf == NULL) {
		printk(KERN_ALERT "No mem on open\n");
		return -ENOMEM;
	}
	mpf->dev = dev;
	mpf->lastoff = 0;

    // set the dev so it is easy to get off the file later
    filp->private_data = mpf;

    return 0;
}

static int map_release(struct inode* node, struct file* filp) {
    printk(KERN_ALERT "Release got called\n");

	if (filp->private_data != NULL) {
		kfree(filp->private_data);
	}

    return 0;
}

static int map_mmap(struct file* filp, struct vm_area_struct* vma) {
    int err = 0;
    unsigned long addr = 0;
	unsigned long savedflg;
	struct map_per_file* mpf;
	struct map_device* dev;

    mpf = (struct map_per_file*)filp->private_data;
	dev = mpf->dev;
    printk(KERN_ALERT "Got Request addr %lx %lx pgoff %lx prot %lx\n", vma->vm_start, vma->vm_end, vma->vm_pgoff, vma->vm_page_prot.pgprot);

    // return if bad request
    if (vma->vm_pgoff != 0) {
        printk("Don't support offset\n");
        return -EINVAL;
    }

    // check size
    if (((vma->vm_end - vma->vm_start) > PAGE_SIZE) || vma->vm_start >= vma->vm_end) {
        printk(KERN_ALERT "Bad mapping size request\n");
        return -EINVAL;
    }
    // check permissions
    if (vma->vm_page_prot.pgprot != PAGE_READONLY.pgprot) {
        printk(KERN_ALERT "Bad mapping permissions request\n");
        return -EINVAL;
    }
    // get a valid addr if they don't have one
    addr = vma->vm_start;
    if (addr <= 0) {
        printk(KERN_ALERT "Bad address, we need to use get_unmapped_area?\n");
        return -EINVAL;
    }
    
    // remap our buffer to them      //maps virtual address to physical address
    err = remap_pfn_range(vma, addr, page_to_pfn(virt_to_page((void*)dev->maparea)), PAGE_SIZE, PAGE_READONLY);
    if (err != 0) {
        printk(KERN_ALERT "Err while trying to map. 0x%x\n", err);
        return err;
    }

	// update the buffer with the user id
	spin_lock_irqsave(&dev->lk, savedflg);
	
	if ((dev->nextoff * sizeof(int)) > (PAGE_SIZE - sizeof(int))) {
		printk(KERN_ALERT "Wrapping around uid accesses\n");
		dev->nextoff = 0;
	}

	if (((int)current->cred->uid.val) == 0) { 
		printk(KERN_ALERT "ignoring uid 0 access\n");
	} else {
		printk(KERN_ALERT "Adding uid %d to ring\n", ((int)current->cred->uid.val));
		((int*)dev->maparea)[dev->nextoff] = (int)(current->cred->uid.val);

		dev->nextoff++;	//points to next chunk of memory for writing
	}
	
	spin_unlock_irqrestore(&dev->lk, savedflg);
	
	// wake up the event queue
	wake_up_interruptible(&dev->queue); //triggers the poll_wait (line 199) and select (line 35) in mapuser.c

    // we could also set vm_operations_struct for operations on thise memory (fault, access, etc)
    // There are some cool things in there

    return 0;
}

static __poll_t map_poll(struct file* filp, struct poll_table_struct* wait) {
	__poll_t mask = 0;
	unsigned long savedflg;
	struct map_per_file* mpf;
	struct map_device* dev;

    mpf = (struct map_per_file*)filp->private_data;
	dev = mpf->dev;

	printk(KERN_ALERT "polled on filp %p\n", filp);

	poll_wait(filp, &dev->queue, wait);

	spin_lock_irqsave(&dev->lk, savedflg);

	if (dev->nextoff != mpf->lastoff) {
		mask |= EPOLLIN | EPOLLRDNORM;
	}

	mpf->lastoff = dev->nextoff;
	spin_unlock_irqrestore(&dev->lk, savedflg);

	// tell what operations could be performed immediately
	return mask;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead");
MODULE_DESCRIPTION("Example map / poll");
module_init(mod_start);
module_exit(mod_stop);
