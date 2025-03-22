#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/major.h>

#define FLAG	"FLAG{iloveiret}"

#define WHATDO_MAJ		246
#define WHATDO_MIN		121

static int whatdo_open(struct inode* node, struct file* filp);
static long whatdo_ioctl(struct file* filp, unsigned int cmd, unsigned long arg);
static loff_t whatdo_llseek(struct file *, loff_t, int);
static ssize_t whatdo_read(struct file *, char __user *, size_t, loff_t *);
static int whatdo_release(struct inode* node, struct file* filp);

struct file_operations whatdoops = {
	.owner = THIS_MODULE,
	.open = whatdo_open,
	.release = whatdo_release,
	.unlocked_ioctl = whatdo_ioctl,
	.llseek = whatdo_llseek,
	.read = whatdo_read,
};

struct whatdo_device {
	struct cdev cdev;
	int state;
};

struct whatdo_device wddev;

static int __init mod_start(void) {
	int err = 0;
	printk(KERN_ALERT "Starting module\n");

	err = register_chrdev_region(MKDEV(WHATDO_MAJ, WHATDO_MIN), 1, "what_do_device_driver");
	if (err != 0) {
		printk(KERN_ALERT "Got error while registering region (%d,%d)\n", WHATDO_MAJ, WHATDO_MIN);
		return err;
	}

	wddev.state = 6;

	cdev_init(&wddev.cdev, &whatdoops);
	err = cdev_add(&wddev.cdev, MKDEV(WHATDO_MAJ, WHATDO_MIN), 1);
	if (err != 0) {
		printk(KERN_ALERT "Got error while adding cdev\n");
		return err;
	}

	printk(KERN_ALERT "Setup device %d:%d\n", WHATDO_MAJ, WHATDO_MIN);

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "module Exiting\n");
		
	unregister_chrdev_region(MKDEV(WHATDO_MAJ, WHATDO_MIN), 1); 

	cdev_del(&wddev.cdev);

	return;
}

static int whatdo_open(struct inode* node, struct file* filp) {
	struct whatdo_device* dev;

	printk(KERN_ALERT "Open got called\n");

	dev = container_of(node->i_cdev, struct whatdo_device, cdev);

	// set the dev so it is easy to get off the file later
	filp->private_data = dev;

	return 0;
}

static int whatdo_release(struct inode* node, struct file* filp) {
	printk(KERN_ALERT "Release got called\n");

	return 0;
}

static long whatdo_ioctl(struct file* filp, unsigned int cmd, unsigned long arg) {
	struct whatdo_device* dev;

	dev = filp->private_data;

	printk(KERN_ALERT "Got ioctl called with command %u and arg %lu and state %d and foff %lld\n", cmd, arg, dev->state, filp->f_pos);

	switch (cmd) {
	case 0:
		if (dev->state == 56 && arg == 27) {
			dev->state = 4;
		}
		break;
	case 1:
		if (dev->state == 3 && arg == 2 && filp->f_pos == 3) {
			dev->state = 0;
		}
		break;
	case 80:
		if (dev->state == 4 && arg == 14) {
			dev->state = 3;
		} 
		break;
	default:
		dev->state += (cmd & 0xff);
		break;
	}	

	return 0;
}

loff_t whatdo_llseek(struct file* filp, loff_t offset, int whence) {
	struct whatdo_device* dev;

	printk(KERN_ALERT "Got lseek called\n");

	dev = filp->private_data;

	if (whence == SEEK_SET && dev->state == 3) {
		// seek based off the beginning
		filp->f_pos = offset;
	}

	return filp->f_pos;
}

static ssize_t whatdo_read(struct file* filp, char __user* buf, size_t size, loff_t* offset) {
	struct whatdo_device* dev;

	printk(KERN_ALERT "Got read of size %lu\n", size);

	dev = filp->private_data;

	if (dev->state != 0) {
		return 0;
	}

	printk(KERN_ALERT "Printing Flag\n");

	dev->state = -1;

	if (size <= 0 || size < sizeof(FLAG)) {
		printk(KERN_ALERT "Read called with too small of buf\n");
		return 0;
	}

	if (copy_to_user(buf, FLAG, sizeof(FLAG))) {
		printk(KERN_ALERT "Failed to copy to user!\n");
		return -EFAULT;
	}

	return sizeof(FLAG);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead");
MODULE_DESCRIPTION("Xor character device");
module_init(mod_start);
module_exit(mod_stop);
