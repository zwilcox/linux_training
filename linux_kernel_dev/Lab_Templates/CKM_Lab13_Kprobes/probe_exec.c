#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>

int before_exec_start(struct kprobe*, struct pt_regs*);
void after_exec_start(struct kprobe*, struct pt_regs*,
				       unsigned long flags);

struct kprobe exec_probe = {
	.symbol_name = /* FIXME: What is the name of the exec syscall symbol? */,
	.pre_handler = before_exec_start,
	.post_handler = after_exec_start,
};

static int __init mod_start(void) {
	int ret;
	ret = /* FIXME: How do I register a kprobe? */;
	if (ret < 0) {
		printk(KERN_ALERT "Failed to register kprobe\n");
		return ret;
	}
	return 0;
}

static void __exit mod_stop(void) {
	/* FIXME: How do I unregister a kprobe */;
	return;
}

int before_exec_start(struct kprobe* p, struct pt_regs* regs) {
	/* FIXME: Do something here... maybe print interesting registers */
	return 0;
}

void after_exec_start(struct kprobe* p, struct pt_regs* regs,
				       unsigned long flags) {
	/* FIXME: Do something interesting here... maybe print interesting registers */
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Kprobes Example");
module_init(mod_start);
module_exit(mod_stop);
