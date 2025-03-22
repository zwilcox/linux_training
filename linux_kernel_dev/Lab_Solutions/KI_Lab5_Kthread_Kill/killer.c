#include <linux/dcache.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/string.h>

struct task_struct *g_kthread;

static int killme_worker(void *data);
static void search_and_destroy(const char *contains);

static int __init mod_start(void) {
  g_kthread =
      kthread_run(killme_worker, (void *)5,
                  "ACTP Daemon"); // kthread_run that wraps kthread_create()
  printk(
      KERN_ALERT
      "Kernel thread started\n"); // killme_worker == thread routine on start-up
  return 0;
}

static void __exit mod_stop(void) {
  kthread_stop(g_kthread);
  printk(KERN_ALERT "Kernel thread terminated\n");
  return;
}

static int killme_worker(void *data) {
  while (!kthread_should_stop()) { // checking to see if thread should be
                                   // terminated - won't be signaled until
                                   // module is terminated
    search_and_destroy("killme");

    // could use schedule here to to a real tight loop
    // dont' set current state as interruptible or uninterruptible first though
    // otherwise you will just sleep
    // schedule();

    // instead, because we are printing a bunch, let's just sleep for a second
    ssleep(1);
  }
  printk(KERN_ALERT "Exiting kernel thread...\n");
  return 0;
}

static void search_and_destroy(const char *contains) {
  struct task_struct *tsk;
  const char *name;
  // find and remove any process containing the str
  for_each_process(tsk) {
    // print both comm and the exe_file name
    printk(KERN_ALERT "comm %s\n", tsk->comm);
    if (tsk->mm && tsk->mm->exe_file != NULL &&
        tsk->mm->exe_file->f_path.dentry != NULL &&
        tsk->mm->exe_file->f_path.dentry->d_name.name != NULL) {
      name = tsk->mm->exe_file->f_path.dentry->d_name.name;
      printk(KERN_ALERT "exec %s\n", name);
      // use this one to compare
      // don't want to kill anything without a exe_file
      if (strstr(name, contains)) {
        // force_sig(SIGKILL, tsk);
        send_sig(SIGKILL, tsk, 0);
      }
    }
  }
}

MODULE_LICENSE("GPL");
module_init(mod_start);
module_exit(mod_stop);
