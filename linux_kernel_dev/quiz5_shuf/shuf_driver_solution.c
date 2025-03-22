#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/major.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include "shuf_shared.h"

loff_t shuf_llseek(struct file *, loff_t, int);
static ssize_t shuf_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t shuf_write(struct file *, const char __user *, size_t, loff_t *);

struct file_operations shufops = {
	.owner = THIS_MODULE,
	.llseek = shuf_llseek,
	.read = shuf_read,
	.write = shuf_write,
};

struct cdev dev;
spinlock_t sl;
char shufbuf[SHUFBUF_SZ];

static int __init mod_start(void) {
	int err = 0;
	printk(KERN_ALERT "Starting shuf module\n");

	err = register_chrdev_region(MKDEV(SHUF_MAJ, SHUF_MIN), 1, "shuf_device_driver");
	if (err != 0) {
		printk(KERN_ALERT "Got error while registering region (%d,%d)\n", SHUF_MAJ, SHUF_MIN);
		return err;
	}

	cdev_init(&dev, &shufops);
	spin_lock_init(&sl);
	err = cdev_add(&dev, MKDEV(SHUF_MAJ, SHUF_MIN), 1);
	if (err != 0) {
		printk(KERN_ALERT "Got error while adding cdev\n");
		return err;
	}

	printk(KERN_ALERT "Created cdev %d:%d\n", SHUF_MAJ, SHUF_MIN);
	
	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "shuf module Exiting\n");
	
	unregister_chrdev_region(MKDEV(SHUF_MAJ, SHUF_MIN), 1);

	cdev_del(&dev);

	return;
}

loff_t shuf_llseek(struct file* filp, loff_t offset, int whence) {
	loff_t newoff = 0;
	if (whence == SEEK_SET) {
		// seek based off the beginning
		newoff = offset;
	} else if (whence == SEEK_CUR) {
		// seek based off the current
		newoff = offset + filp->f_pos;
	} else if (whence == SEEK_END) {
		// seek based off the end
		newoff = SHUFBUF_SZ + offset;
	}

	if (newoff > SHUFBUF_SZ) {
		newoff = SHUFBUF_SZ;
	}

	if (newoff < 0) {
		newoff = 0;
	}

	filp->f_pos = newoff;

	printk(KERN_ALERT "llseek was called, new offset is %lld\n", newoff);

	return newoff;
}

static ssize_t shuf_read(struct file* filp, char __user* buf, size_t size, loff_t* offset) {
	ssize_t len = min((ssize_t)(SHUFBUF_SZ - *offset), (ssize_t)size);

	printk(KERN_ALERT "Got read of size %lu\n", size);

	if (len <= 0) {
		return 0;
	}

	spin_lock(&sl);
	if (copy_to_user(buf, shufbuf + *offset, len)) {
		spin_unlock(&sl);
		printk(KERN_ALERT "Failed to copy to user!\n");
		return -EFAULT;
	}
	spin_unlock(&sl);

	*offset += len;

	return len;
}

static ssize_t shuf_write(struct file* filp, const char __user* buf, size_t size, loff_t* offset) {
	int i;
	char tmpbuf[SHUFBUF_SZ];
	ssize_t len = min((ssize_t)(SHUFBUF_SZ - *offset), (ssize_t)size);

	printk(KERN_ALERT "Got write of size %lu\n", size);

	if (len <= 0) {
		return 0;
	}

	if (copy_from_user(tmpbuf + *offset, buf, len)) {
		printk(KERN_ALERT "Failed to copy from user!\n");
		return -EFAULT;
	}

	// xor it and xor it into the real buf
	spin_lock(&sl);
 	for (i=0; i<len; i++) {
		shufbuf[i + *offset] ^= tmpbuf[i + *offset];
	}
	spin_unlock(&sl);

	*offset += len;

	return len;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead");
MODULE_DESCRIPTION("Shuf character device");
module_init(mod_start);
module_exit(mod_stop);
