#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/major.h>
#include "xorshared.h"

#define DEFAULT_KEY         0x42

static int xor_open(struct inode* node, struct file* filp);
static int xor_release(struct inode* node, struct file* filp);
static long xor_ioctl(struct file* filp, unsigned int cmd, unsigned long arg);
loff_t xor_llseek(struct file *, loff_t, int);
static ssize_t xor_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t xor_write(struct file *, const char __user *, size_t, loff_t *);

struct file_operations xorops = { //sets up the operations we want to associate with our device (in this case a character device)
    .owner = THIS_MODULE,
    .open = xor_open,
    .release = xor_release,
    .unlocked_ioctl = xor_ioctl,
    .llseek = xor_llseek,
    .read = xor_read,
    .write = xor_write,
};

struct xor_device {
    struct cdev screwitall; //kernel specific for device type (in this case - character device)
    char key;
    char xorbuf[XOR_BUF_SZ];
};

struct xor_device dev;

static int __init mod_start(void) {
    int err = 0;
    printk(KERN_ALERT "Starting xor module\n");
    
    err = register_chrdev_region(MKDEV(XOR_MAJ, XOR_MIN), 1, "xor_device_driver"); // 1 == # of entries to register for
    if (err != 0) {
        printk(KERN_ALERT "Got error while registering region (%d,%d)\n", XOR_MAJ, XOR_MIN);
        return err;
    }

	dev.key = DEFAULT_KEY;
	cdev_init(&dev.cdev, &xorops);
	err = cdev_add(&dev.cdev, MKDEV(XOR_MAJ, XOR_MIN), 1);
	if (err != 0) {
		printk(KERN_ALERT "Got error while adding cdev\n");
		return err;
    }

    printk(KERN_ALERT "Setup device\n");

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "xor module Exiting\n");
	cdev_del(&dev.cdev);
	unregister_chrdev_region(MKDEV(XOR_MAJ, XOR_MIN), 1);
	return;
}

static int xor_open(struct inode* node, struct file* filp) {
    struct xor_device* dev;

    printk(KERN_ALERT "Open got called\n");
    //ptr to top of target   //*type         //target    //name within target
    dev = container_of(node->i_cdev, struct xor_device, cdev);

    // set the dev so it is easy to get off the file later
    filp->private_data = dev;

    return 0;
}

static int xor_release(struct inode* node, struct file* filp) {
    printk(KERN_ALERT "Release got called\n");

    return 0;
}

static long xor_ioctl(struct file* filp, unsigned int cmd, unsigned long arg) {
    struct xor_device* dev;

    printk(KERN_ALERT "Got ioctl called with command %u and arg %lu\n", cmd, arg);

    dev = filp->private_data;

    if (cmd == KEYCMD) {
        dev->key = (char)arg;
        printk(KERN_ALERT "Key is now %02X\n", dev->key);
    }

    return 0;
}

loff_t xor_llseek(struct file* filp, loff_t offset, int whence) {
    loff_t newoff = 0;
    if (whence == SEEK_SET) {
        // seek based off the beginning
        newoff = offset;
    } else if (whence == SEEK_CUR) {
        // seek based off the current
        newoff = offset + filp->f_pos;
    } else if (whence == SEEK_END) {
        // seek based off the end
        newoff = XOR_BUF_SZ + offset;
    }

    if (newoff > XOR_BUF_SZ) {
        newoff = XOR_BUF_SZ;
    }

    if (newoff < 0) {
        newoff = 0;
    }

    filp->f_pos = newoff;

    printk(KERN_ALERT "llseek was called, new offset is %lld\n", newoff);

    return newoff;
}

static ssize_t xor_read(struct file* filp, char __user* buf, size_t size, loff_t* offset) {
    struct xor_device* dev;
    ssize_t len = min((ssize_t)(XOR_BUF_SZ - *offset), (ssize_t)size);

    printk(KERN_ALERT "Got read of size %lu\n", size);

    dev = filp->private_data;

    if (len <= 0) {
        return 0;
    }
                    //dst      //src
    if (copy_to_user(buf, dev->xorbuf + *offset, len)) {
        printk(KERN_ALERT "Failed to copy to user!\n");
        return -EFAULT;
    }

    *offset += len;

    return len;
}

static ssize_t xor_write(struct file* filp, const char __user* buf, size_t size, loff_t* offset) {
    int i;
    struct xor_device* dev;
    ssize_t len = min((ssize_t)(XOR_BUF_SZ - *offset), (ssize_t)size);

    printk(KERN_ALERT "Got write of size %lu\n", size);

    dev = filp->private_data;

    if (len <= 0) {
        return 0;
    }

    if (copy_from_user(dev->xorbuf + *offset, buf, len)) {
        printk(KERN_ALERT "Failed to copy from user!\n");
        return -EFAULT;
    }

    // xor it
    for (i = 0; i < len; i++) {
        dev->xorbuf[*offset+i] ^= dev->key;
    }

    *offset += len;

    return len;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead and Mark O'Neill");
MODULE_DESCRIPTION("Xor character device");
module_init(mod_start);
module_exit(mod_stop);
