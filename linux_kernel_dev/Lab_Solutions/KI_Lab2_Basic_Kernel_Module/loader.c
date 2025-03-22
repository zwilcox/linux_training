#include <linux/module.h>

#define RING_LEVEL(x) (x & 0x3) // used to print out current ring level

static char *welcome_msg = "Hello World"; // must define as global static
static int show_ring = 0;                 // these are passed to insmod()

static int get_ring_level(void);
module_param(welcome_msg, charp, 0);
module_param(show_ring, int, 0);

static int __init mod_start(void) {
  printk(KERN_ALERT "%s\n", welcome_msg);
  if (show_ring == 1) {
    printk(KERN_ALERT "The ring level is %d\n", get_ring_level());
  }
  return 0; // kernel module is still loaded (but done running)
}

// must run 'rmmod()' to remove module

static void __exit mod_stop(void) {
  printk(KERN_ALERT "Module Exiting\n");
  return 0; // used to cleanup anything associated with the module (ie. memory)
}

static int get_ring_level(void) {
  int cs_reg;
  asm("mov %%cs, %[x]" : [ x ] "=r"(cs_reg));
  return RING_LEVEL(cs_reg);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Hello World Demo");
module_init(mod_start);
module_exit(mod_stop);
