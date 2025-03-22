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
    tsk = /* FIXME: I have a PID, how can I find the task struct */;
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
		// HINT: Some regions of memory are backed by a file, how can we tell if a file backs
        // a section. How can we find the name of the file that is backing that section?

		// FIXME: check a member of the VM struct looking for things like files, dentries, names, etc
			
		// what function could I use to change the string - navigate Elixir so you know why!
		// could use that function read the memory out here
		?some_func?(/*FIXME: what do I need to pass to read memory? */);
		// could use that same function to set the memory
		?some_func?(/*FIXME: What do I need to pass to change memory? */);

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
