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
    // TODO: add functionalitiy
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

    // TODO: register region

	spin_lock_init(/* TODO: */); 
	init_waitqueue_head(/* TODO: */);
	mdev.nextoff = // TODO:
    mdev.maparea = // TODO:  //might use kmalloc or or get_zeroed_page

    // TODO: initialize device
    // TODO: add device

    printk(KERN_ALERT "Setup device %d:%d\n", MAP_MAJ, MAP_MIN);

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "map module Exiting\n");
	
    // TODO: unregister region
    // TODO: delete device

    free_pages((unsigned long)mdev.maparea, 0);

	return;
}

static int map_open(struct inode* node, struct file* filp) {
    struct map_device* dev;
	struct map_per_file* mpf;

    printk(KERN_ALERT "Open got called\n");

    dev = container_of(/* TODO: */);

    // TODO: call kmalloc
	mpf->dev = // TODO:
	mpf->lastoff // TODO:

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

    // vma checks (optional, but good practice):
    /*
        - check request
        - check size
        - check permissions
    */

    // get a valid addr if they don't have one
    addr = vma->vm_start;
    
    // remap our buffer to them - and maps virtual address to physical address
    // use: remap_pfn_range(), page_to_pfn(), virt_to_page()

	spin_lock_irqsave(/* TODO: */);
	
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
	
	spin_unlock_irqrestore(/* TODO: */);
	
	// TODO: wake up the event queue

    // we could also set vm_operations_struct for operations on this memory (fault, access, etc)
    // there are some cool things in there

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

	poll_wait(/* TODO: */);

	spin_lock_irqsave(/* TODO: */);

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
