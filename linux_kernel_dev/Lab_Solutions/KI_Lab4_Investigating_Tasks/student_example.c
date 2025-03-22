#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NVIMDEMON");
MODULE_DESCRIPTION("Task Family Tree Traversal");
MODULE_VERSION("1.0");

void print_task_info(struct task_struct *task, int depth)
{
    struct task_struct *child;
    struct list_head *list;

    printk(KERN_INFO "PID: %-5d | Name: %-16s | mm: %px | thread_pid: %p | state: %-4ld | sp: %p\n",
        task->pid, task->comm, task->mm, task->thread_pid, task->state, (void *)task->thread.sp);

    list_for_each(list, &task->children)
    {
        child = list_entry(list, struct task_struct, sibling);
        print_task_info(child, depth + 1);
    }
}

static int __init task_tree_init(void)
{
    printk(KERN_INFO "Loading FINDTASKS\n");

    print_task_info(&init_task, 0);

    return 0;
}

static void __exit task_tree_exit(void)
{
    printk(KERN_INFO "Unloading FINDTASKS\n");
}

module_init(task_tree_init);
module_exit(task_tree_exit);
