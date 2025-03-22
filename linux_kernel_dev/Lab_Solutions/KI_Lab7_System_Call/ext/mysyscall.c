#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

#include "mysyscall.h"

SYSCALL_DEFINE0(helloworld) {
  printk(KERN_ALERT "Hello, World!\n");
  return 0;
}
