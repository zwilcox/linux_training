#include <linux/module.h> /* LKM */
#include <linux/printk.h> /* printing */
#include <linux/miscdevice.h> /* sysfs udev devices */
#include <linux/fs.h> /* file_operations */
#include <linux/slab.h> /* kmalloc/kfree/etc */
#include <linux/uaccess.h> /* copy_to_user, copy_from_user */
#include <asm/page.h> /* virt_addr_valid */
#include <linux/cred.h> /* cred members */

#define ROOTHELPER_DEV_NAME	"roothelper"

static int rhelper_release(struct inode *inode, struct file *file);
static int rhelper_open(struct inode *inode, struct file *file);
static ssize_t rhelper_read(struct file* file, char __user* buffer, size_t count, loff_t* ppos);
static ssize_t rhelper_write(struct file* file, const char __user* buffer, size_t count, loff_t* ppos);
static loff_t rhelper_llseek(struct file *file, loff_t offset, int whence);

struct file_operations rhelper_fops = {
	.owner = THIS_MODULE,
	.open = rhelper_open,
	.release = rhelper_release,
	.llseek = rhelper_llseek,
	.read = rhelper_read,
	.write = rhelper_write,
};

struct rhelper {
	void* kptr;
};

static struct miscdevice rhelper_misc = {
	.fops = &rhelper_fops,
	.minor = MISC_DYNAMIC_MINOR,
	.name = ROOTHELPER_DEV_NAME,
};

static int rhelper_open(struct inode* inode, struct file* file) {
	struct rhelper* rh;
	rh = kzalloc(sizeof(struct rhelper), GFP_KERNEL);
	if (rh == NULL) {
		return -ENOMEM;
	}

	file->private_data = rh;

	generic_file_open(inode, file);
	printk(KERN_INFO "opened a root helper\n");
	return 0;
}

static int rhelper_release(struct inode* inode, struct file* file) {
	struct rhelper* rh = (struct rhelper*)file->private_data;
	kfree(rh);
	printk(KERN_INFO "released a root helper\n");
	return 0;
}

static ssize_t rhelper_read(struct file* file, char __user* buffer, size_t count, loff_t* ppos) {
	struct rhelper* rh = (struct rhelper*)file->private_data;
	if (count == 0) {
		return -EINVAL;
	}
	if (!virt_addr_valid(rh->kptr)) {
		return -EFAULT;
	}
	if (copy_to_user(buffer, rh->kptr, count) != 0) {
		return -EFAULT;
	}
	return count;
}

static ssize_t rhelper_write(struct file* file, const char __user* buffer, size_t count, loff_t* ppos) {
	struct rhelper* rh = (struct rhelper*)file->private_data;
	if (count == 0) {
		return -EINVAL;
	}
	if (!virt_addr_valid(rh->kptr)) {
		return -EFAULT;
	}
	if (copy_from_user(rh->kptr, buffer, count) != 0) {
		return -EFAULT;
	}
	return count;
}

static loff_t rhelper_llseek(struct file *file, loff_t offset, int whence) {
	struct rhelper* rh = (struct rhelper*)file->private_data;
	if (whence == SEEK_CUR) {
		return (loff_t)current;
	}
	else {
		rh->kptr = (void*)offset;
		printk(KERN_ALERT "Next address to be read/written: %px\n", rh->kptr);
	}
	return offset;
}

static int __init mod_start(void) {
	int err;
	err = misc_register(&rhelper_misc);
	if (err == 0) {
		printk(KERN_ALERT "Root helper loaded\n");
	}
	return err;
}

static void __exit mod_stop(void) {
	misc_deregister(&rhelper_misc);
	printk(KERN_ALERT "Root helper unloaded\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Module that grants arbitrary kernel R/W and init_task peek");
module_init(mod_start);
module_exit(mod_stop); 
