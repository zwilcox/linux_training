#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/tcp.h> /* for TCP functions */
#include <linux/ctype.h> /* for isalpha */
#include <linux/socket.h> /* for msghdr */
#include <linux/uio.h> /* iters */

#include "conn_state.h"

static struct proto orig_tcp_prot;
extern struct proto tcp_prot;

static int new_tcp_sendmsg(struct sock *sk, struct msghdr *msg, size_t size);
static int new_tcp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, 
	int nonblock, int flags, int *addr_len);
static int new_tcp_v4_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len);
static void new_tcp_close(struct sock *sk, long timeout);
static struct sock * new_inet_csk_accept(struct sock *sk, int flags, int *err, bool kern);

static int __init mod_start(void) {
	printk(KERN_ALERT "Hello, World\n");
	orig_tcp_prot = tcp_prot;
	/* FIXME: We're interested in hooking TCP, where are those handler functions? */

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	/* FIXME: Make sure we put the pointers back! */
	conn_state_delete_all();

	return;
}

static int new_tcp_sendmsg(struct sock *sk, struct msghdr *msg, size_t size) {
	int ret;
	conn_state_t* cs;

	printk(KERN_ALERT "Hooked TCP Send\n");
	cs = /* FIXME: Get a connection state */;
	if (cs != NULL) {
		printk(KERN_ALERT "Got a send from an ncat socket\n");
	}
	ret = /* FIXME: Send message */;

	return ret;
}

static int new_tcp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, 
	int nonblock, int flags, int *addr_len) {
	size_t i;
	int ret;
	conn_state_t* cs;
	size_t iov_len;
	char* iov_base;
	unsigned char* new_buffer;

	printk(KERN_ALERT "Hooked TCP Receive\n");
	cs = /* FIXME: Get a connection state */;
	if (cs != NULL) {
	    iov_len = /* FIXME: This is a connection we care about, where is our data? */;
		iov_base = /* FIXME: This is a connection we care about, where is our data? */;
	}

	ret = /* FIXME: Get the data */;

	if (cs != NULL) {
		printk(KERN_ALERT "Got a recv from an ncat socket\n");
		if (ret <= 0) { /* if there's no data to screw up let's stop */
			return ret;
		}
		if (iov_len == 0) {
			return ret;
		}
	    /* How can we modify the data before we ship it on? */
		new_buffer = kmalloc(/* FIXME */);
		if (new_buffer == NULL) {
			return ret;
		}
		if (copy_from_user(/* FIXME */) != 0) {
			printk(KERN_ALERT "Copy from user %p failed\n", iov_base);
			return ret;
		}
		/* Let's modify the data! */
		for (i = 0; i < ret; i++) {
			new_buffer[i] = toupper(new_buffer[i]);
		}
		if (copy_to_user(/* FIXME */) != 0) {
			printk(KERN_ALERT "Copy from user %p failed\n", iov_base);
			return ret;
		}
		kfree(new_buffer);
	}
	return ret;
}

int new_tcp_v4_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len) {
	int ret;

	printk(KERN_ALERT "Hooked TCP Connect for %s\n", current->comm);
	ret = /* FIXME: Connect */;

	return ret;
}

void new_tcp_close(struct sock *sk, long timeout) {
	conn_state_t* cs;

	printk(KERN_ALERT "Hooked TCP Close\n");
	cs = /* FIXME: Get a connection state */;
	if (cs != NULL) {
		printk(KERN_ALERT "Got a close from an ncat socket\n");
	}
	/* FIXME: Close the connection */;

	return;
}

static struct sock * new_inet_csk_accept(struct sock *sk, int flags, int *err, bool kern) {
	conn_state_t* cs;
	struct sock* ret;

	printk(KERN_ALERT "Hooked TCP Accept\n");
	ret = /* FIXME: Accept a connection */;
	if (strcmp(current->comm, "ncat") == 0) { /* Note the use of `current` here */
		printk(KERN_ALERT "Found an accepted connection from ncat\n");
		if (ret == NULL) {
			return ret;
		}
		cs = /* FIXME: Get a connection state */;
		if (cs == NULL) {
			cs = /* FIXME: Create a connection state */;
			printk(KERN_ALERT "Got an accept from a new ncat socket\n");
		}
		else {
			printk(KERN_ALERT "This should never happen\n");
		}
	}
	
	return ret;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Network Hijacking Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 

