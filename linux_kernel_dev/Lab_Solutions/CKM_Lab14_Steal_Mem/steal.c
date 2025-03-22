#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dcache.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>

#define BUFOFFS     	0x4080 
#define BUFSIZE     	0x100
#define MAX_FILENAME	0x400
#define NEWVALUE		"ALL_FIXED"

static pid_t target = 0;
module_param(target, int, 0);

static char filename[MAX_FILENAME] = {0};
module_param_string(filename, filename, MAX_FILENAME-1, 0);

static int __init mod_start(void) {
	char buffer[256];
    struct task_struct* tsk;
    struct vm_area_struct* vms;

   	printk(KERN_ALERT "Module Starting. Pid = %d\n", target);
    if (target == 0) {
        printk(KERN_ALERT "Please specify a pid\n");
        return -EINVAL;
    }

	if (filename[0] == '\0') {
		printk(KERN_ALERT "Please specify a filename\n");
		return -EINVAL;
	}
    
    // Find the task struct for the pid
    tsk = pid_task(find_vpid(target), PIDTYPE_PID);
    if (tsk == NULL) {
        printk(KERN_ALERT "Unable to find task\n");
        return -ESRCH;
    }

    // Read from that process's memory
    printk(KERN_ALERT "looking for the mapped file\n");

    if (tsk->mm == NULL) {
        printk(KERN_ALERT "Tried to work on a bad pid! No mm_struct\n");
        return -EINVAL;
    }

    vms = tsk->mm->mmap;

    if (vms == NULL) {
        printk(KERN_ALERT "No associated mmap with this mm_struct\n");
        return -EINVAL;
    }
    
    // ok so the vms->vm_next/prev is in a linked list sorted by address
    // I am just going to go the the beginning of this and then search from there
    while (vms->vm_prev != NULL) {
        vms = vms->vm_prev;
    }

    // find where the image is mapped
   for (; vms != NULL; vms = vms->vm_next) {
		if (vms->vm_file) {
			if (vms->vm_file->f_path.dentry == NULL) {
				printk(KERN_ALERT "Null dentry\n");
				continue;
			}
			if (vms->vm_file->f_path.dentry->d_name.name == NULL) {
				printk(KERN_ALERT "Null name\n");
				continue;
			}
			if (strcmp(vms->vm_file->f_path.dentry->d_name.name, filename)) {
				continue;
			}
			printk(KERN_ALERT "File at 0x%016lx %s\n", vms->vm_start, vms->vm_file->f_path.dentry->d_name.name);
			
			// use access_process_vm to change the string
			// could read it out here
			access_process_vm(tsk, vms->vm_start + BUFOFFS, buffer, sizeof(buffer), FOLL_FORCE);

			// set it
			access_process_vm(tsk, vms->vm_start + BUFOFFS, NEWVALUE, sizeof(NEWVALUE), FOLL_WRITE | FOLL_FORCE);

			break;
		}
	}
    printk(KERN_ALERT "Done\n");

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Module Exiting\n");

	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead");
MODULE_DESCRIPTION("MyMemPeek");
module_init(mod_start);
module_exit(mod_stop);
