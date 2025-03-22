#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

#include "common.h"

#define KEYBOARD_IRQ		0x00 /* FIXME: Which value represents the keyboard irq */
#define I8042_STATUS_REG	0x00 /* FIXME: Which value represents the status register */
#define I8042_DATA_REG		0x00 /* FIXME: Which value represents the data register */
#define RING_BUFFER_SIZE	128

/* Structs and prototypes */
struct key_device {
	struct cdev cdev;
	struct semaphore sem;
	unsigned int rring_idx;
	unsigned int wring_idx;
	unsigned short ring_buf[RING_BUFFER_SIZE];
};

static ssize_t key_read(struct file* filp, char __user* buf, size_t size, loff_t* offset);
static int register_dev(void);
static irqreturn_t int_handler(int irq_nr, void* dev_id);

/* Global vars */
struct key_device kdev;

static struct file_operations key_ops = {
	/* FIXME: populate struct */
};


static int __init mod_start(void) {
	int err;
	err = register_dev();
	if (err != 0) {
		return err;
	}

	err = /* FIXME: How do I request the IRQ? */;
	if (err != 0) {
		/* FIXME: How do I delete my device, and unregister the device region if I hit an error? */
		return err;
	}

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	/* FIXME: How do I cleanup? */
	
	return;
}

int register_dev(void) {
	int err;

	err = /* FIXME: How do I register a chrdev_region? */;
	if (err != 0) {
		printk(KERN_ALERT "Could not register region (%d,%d)\n", KEY_MAJ, KEY_MIN);
		return err;
	}

	/* TODO: intialize your character device */
	err = /* FIXME: how do I add the cdev? */;
	if (err != 0) {
		printk(KERN_ALERT "Error adding char device: %d\n", err);
		return err;
	}

	kdev.wring_idx = 0;
	kdev.rring_idx = 0;
	sema_init(&kdev.sem, 0);

	return 0;
}

ssize_t key_read(struct file* filp, char __user* buf, size_t size, loff_t* offset) {
	unsigned short keycode;

	if (down_interruptible(&kdev.sem) == -EINTR) {
		return -EINTR;
	}
	keycode = /* FIXME: how do I read stuff from my ring buffer, get it to the user? */ 

	if (copy_to_user(/* FIXME: what goes here? */) != 0) {
		return 0;
	}
	kdev.rring_idx++;

	return sizeof(unsigned short);
}

irqreturn_t int_handler(int irq_nr, void* dev_id) {
	struct key_device* dev = (struct key_device*)dev_id;
	unsigned short keycode;
	
	/* FIXME: how do I get data from the ports/store it in the ring buffer? */; 
	/* Hint, shifting the control or key data might help - checkout inb() */
	printk(KERN_ALERT "Code: %04x\n", keycode);
	up(&dev->sem);

	return IRQ_NONE;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Keylogger (interrupt) Lab Solution");
module_init(mod_start);
module_exit(mod_stop);
