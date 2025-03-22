#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>

int before_exec_start(struct kprobe*, struct pt_regs*);
void after_exec_start(struct kprobe*, struct pt_regs*,
				       unsigned long flags);

struct kprobe exec_probe = {
	.symbol_name = "__x64_sys_execve",
	.pre_handler = before_exec_start,
	.post_handler = after_exec_start,
};

static int __init mod_start(void) {
	int ret;
	ret = register_kprobe(&exec_probe);
	if (ret < 0) {
		printk(KERN_ALERT "Failed to register kprobe\n");
		return ret;
	}
	return 0;
}

static void __exit mod_stop(void) {
	unregister_kprobe(&exec_probe);
	return;
}

int before_exec_start(struct kprobe* p, struct pt_regs* regs) {
	printk(KERN_ALERT "Before start of do_exec, RSP = %#016lx\n", regs->sp);
	return 0;
}

void after_exec_start(struct kprobe* p, struct pt_regs* regs,
				       unsigned long flags) {
	printk(KERN_ALERT "After start of do_exec, RSP = %#016lx\n", regs->sp);
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Kprobes Example");
module_init(mod_start);
module_exit(mod_stop); 

