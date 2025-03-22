#include...

#define DEFAULT_KEY 0x42

// TODO: functions to implement
static int xor_open(struct inode *node, struct file *filp);
static int xor_release(struct inode *node, struct file *filp);
static long xor_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
loff_t xor_llseek(struct file *, loff_t, int);
static ssize_t xor_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t xor_write(struct file *, const char __user *, size_t, loff_t *);

// TODO: how do we add functionality?
// TODO: is there a good way to manage the device, key, and buffer?
// TODO: register, initialize, and add your device - where would I do this?
// TODO: delete and unregister your device - where would I do this?

static int xor_open(struct inode *node, struct file *filp) {
  struct xor_device *dev;

  printk(KERN_ALERT "Open got called\n");

  dev = container_of(/* TODO:...*/);

  // set the dev so it is easy to get off the file later
  filp->private_data = dev;

  return 0;
}

static int xor_release(struct inode *node, struct file *filp) {
  printk(KERN_ALERT "Release got called\n");
  return 0;
}

static long xor_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  struct xor_device *dev;

  printk(KERN_ALERT "Got ioctl called with command %u and arg %lu\n", cmd, arg);

  // Remember, we stored the structure that represents our device in the private
  // data of the file structure.

  if (cmd == KEYCMD) {
    // FIXME: We got a command to update the xor key, where do we store it?
  }

  return 0;
}

loff_t xor_llseek(struct file *filp, loff_t offset, int whence) {
  loff_t newoff = 0;
  if (whence == SEEK_SET) {
    // FIXME: seek based off the beginning
  } else if (whence == SEEK_CUR) {
    // FIXME: seek based off the current
  } else if (whence == SEEK_END) {
    // FIXME: seek based off the end
  }

  if (newoff > XOR_BUF_SZ) {
    newoff = XOR_BUF_SZ;
  }

  if (newoff < 0) {
    newoff = 0;
  }

  filp->f_pos = newoff;

  printk(KERN_ALERT "llseek was called, new offset is %lld\n", newoff);

  return newoff;
}

// Note: by the time we get a read, we get the offset into the file and a size,
// *WE* are responsible for figuring out how to get this data.
static ssize_t xor_read(struct file *filp, char __user *buf, size_t size,
                        loff_t *offset) {
  struct xor_device *dev;
  // FIXME: We should probably make sure that our read won't go past the size of
  // our buffer.

  printk(KERN_ALERT "Got read of size %lu\n", size);

  // NOTE: remember we stored our device in the private data of the file struct,
  // we may want to get that back out.

  // If we asked to read less than 1 byte, lets handle the trivial case here
  if (len <= 0) {
    return 0;
  }

  // FIXME: How do I get data to userspace again?

  // FIXME: We need to update the offset variable with how many bytes we
  // actually returned

  // FIXME: we need to return how many bytes we actually sent to the user
}

static ssize_t xor_write(struct file *filp, const char __user *buf, size_t size,
                         loff_t *offset) {
  int i;
  struct xor_device *dev;
  // FIXME: We should probably make sure that our write won't go past the size
  // of our buffer.

  printk(KERN_ALERT "Got write of size %lu\n", size);

  // NOTE: remember we stored our device in the private data of the file struct,
  // we may want to get that back out.

  // If we asked to write less than 1 byte, lets handle the trivial case here
  if (len <= 0) {
    return 0;
  }

  // FIXME: how do we get data from the user into our buffer?

  // FIXME: xor the data (perhaps in a for loop?)

  // FIXME: We need to update the offset with how many bytes were actually
  // written

  // FIXME: We need to return how many bytes were actually written.
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jordan Whitehead and Mark O'Neill");
MODULE_DESCRIPTION("Xor character device");
module_init(mod_start);
module_exit(mod_stop);
