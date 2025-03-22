#include <asm/processor.h>
#include <asm/ptrace.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define RSP 19 * 8

static void list_children(struct task_struct *task, int tab);

static int __init mod_start(void) {
  printk(KERN_ALERT "Hello, World\n");
  list_children(&init_task, 0); // can use init_task to grab first task
  return 0;
}

static void __exit mod_stop(void) {
  printk(KERN_ALERT "Goodbye, World\n");
  return;
}

/* stolen function from ptrace API (not exported) */
static unsigned long *pt_regs_access(struct pt_regs *regs,
                                     unsigned long offset) {
  BUILD_BUG_ON(offsetof(struct pt_regs, r15) != 0);
  return &regs->r15 + (offset / sizeof(regs->r15));
}

static void list_children(struct task_struct *task, int tab) {
  static char tab_buf[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t";
  struct task_struct *child;
  struct task_struct *thread;
  struct list_head *tmp;
  struct list_head *list;
  printk(KERN_ALERT "%.*s-----------------------------------------\n", tab,
         tab_buf);
  printk(KERN_ALERT "%.*s%s [PID: %d TID %p threads: %d]\n", tab, tab_buf,
         task->comm, task->pid, task->thread_pid, get_nr_threads(task));
  for_each_thread(task, thread) {
    printk(KERN_ALERT "%.*sThread PID: %d\n", tab, tab_buf, thread->pid);
  }
  printk(KERN_ALERT "%.*smm: %px RSP: %08lX\n", tab, tab_buf, task->mm,
         *pt_regs_access(task_pt_regs(task), RSP));
  printk(KERN_ALERT "%.*s-----------------------------------------\n", tab,
         tab_buf);

  list_for_each_safe(list, tmp, &task->children) {
    child = list_entry(list, struct task_struct, sibling);
    list_children(child, tab + 1);
  }
  return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Process Management Lab Solution");
module_init(mod_start);
module_exit(mod_stop);
