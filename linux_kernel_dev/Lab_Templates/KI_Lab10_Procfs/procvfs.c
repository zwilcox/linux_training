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
	// the kernel has a path struct that we can use to help us find a file, this gets
	// a path struct that represents the root of /proc
	if (kern_path("/proc", 0, &proc_path) != 0) {
		printk(KERN_ALERT "Failed to get path to /proc\n");
	}
	// The proc_path has a dentry in it, I should save the original file
	// operations because I'm about to hook one of them and I would like to put
	// it back. Where are those file operations stored?
	orig_fops = /* FIXME: Where can I find those file operations? */;

	// Now I want to save a pointer to the original iterate_shared function for convenience
	orig_iterate_shared = /* FIXME: Where is that iterate_shared member? */;

	// Lets copy over the original fops structure to my local new_fops structure
	new_fops = /* FIXME: Where were those pesky file operations again, I had them a line or two above! */;
	new_fops.iterate_shared = new_iterate_shared; // Install my new iterate_shared function

	/* Now we add the pointer to my new fileops back where we found the original */ 
	proc_path.dentry->d_inode->i_fop = &new_fops;
	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	// FIXME: Put the original inode's file operations back or we're going to have a bad time
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Proc/VFS Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 


int new_filldir_t(struct dir_context* d_ctx, const char* name, int len,
		loff_t off, u64 ino, unsigned int d_type) {
	// If this is our target to hide, we want to skip it, how do we test
	// that, and what should we return if this is our target?
	// If you statically compile in the process to hide, make sure it is a long-lived process
	if (/* FIXME */ ) {
		printk(KERN_ALERT "Process %s hidden\n", "<PID_STRING>");
		return /* FIXME: what should I return? */;
	}
	// If this isn't our target, what should we do?  (what function did we save?)
	return /* FIXME: What should we do here? */ ;
}

// This function is called as we are iterating over the directory structure
int new_iterate_shared(struct file* f, struct dir_context* d_ctx) {
	int ret;
	struct dir_context new_ctx = {
		.actor = /* FIXME: What function of ours do we want to call as we are iterating over the directory? */,
		.pos = d_ctx->pos
	};
	// We need to save our original d_ctx so our hook can call the original actor
	orig_d_ctx = d_ctx;
	ret = /* FIXME: How do we call the original iterate_shared function? */;
	// Update the dir_context we were passed with the result of the hooked operation
	d_ctx->pos = new_ctx.pos;
	return ret;
}
