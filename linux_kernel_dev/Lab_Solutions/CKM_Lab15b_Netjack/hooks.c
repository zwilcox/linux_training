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
	tcp_prot.recvmsg = new_tcp_recvmsg;
	tcp_prot.sendmsg = new_tcp_sendmsg;
	tcp_prot.close = new_tcp_close;
	tcp_prot.connect = new_tcp_v4_connect;
	tcp_prot.accept = new_inet_csk_accept;

	return 0;
}

static void __exit mod_stop(void) {
	printk(KERN_ALERT "Goodbye, World\n");
	tcp_prot = orig_tcp_prot;
	conn_state_delete_all();
	return;
}

static int new_tcp_sendmsg(struct sock *sk, struct msghdr *msg, size_t size) {
	int ret;
	conn_state_t* cs;

	printk(KERN_ALERT "Hooked TCP Send\n");
	cs = conn_state_get((unsigned long)sk);
	if (cs != NULL) {
		printk(KERN_ALERT "Got a send from an ncat socket\n");
	}
	ret = orig_tcp_prot.sendmsg(sk, msg, size);

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
	cs = conn_state_get((unsigned long)sk);
	if (cs != NULL) {
		iov_len = msg->msg_iter.iov->iov_len;
		iov_base = (char*)msg->msg_iter.iov->iov_base;
	}

	ret = orig_tcp_prot.recvmsg(sk, msg, len, nonblock, flags, addr_len);

	if (cs != NULL) {
		printk(KERN_ALERT "Got a recv from an ncat socket\n");
		if (ret <= 0) { /* if there's no data to screw up let's stop */
			return ret;
		}
		if (iov_len == 0) {
			return ret;
		}
		new_buffer = kmalloc(iov_len, GFP_KERNEL);
		if (new_buffer == NULL) {
			return ret;
		}
		if (copy_from_user(new_buffer, iov_base, ret) != 0) {
			printk(KERN_ALERT "Copy from user %p failed\n", iov_base);
			return ret;
		}
		for (i = 0; i < ret; i++) {
			new_buffer[i] = toupper(new_buffer[i]);
		}
		if (copy_to_user(iov_base, new_buffer, ret) != 0) {
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
	ret = orig_tcp_prot.connect(sk, uaddr, addr_len);

	return ret;
}

void new_tcp_close(struct sock *sk, long timeout) {
	conn_state_t* cs;
	printk(KERN_ALERT "Hooked TCP Close\n");
	cs = conn_state_get((unsigned long)sk);
	if (cs != NULL) {
		printk(KERN_ALERT "Got a close from an ncat socket\n");
	}
	orig_tcp_prot.close(sk, timeout);

	return;
}

static struct sock * new_inet_csk_accept(struct sock *sk, int flags, int *err, bool kern) {
	conn_state_t* cs;
	struct sock* ret;

	printk(KERN_ALERT "Hooked TCP Accept\n");
	ret = orig_tcp_prot.accept(sk, flags, err, kern);
	if (strcmp(current->comm, "ncat") == 0) {
		printk(KERN_ALERT "Found an accepted connection from ncat\n");
		if (ret == NULL) {
			return ret;
		}
		cs = conn_state_get((unsigned long)ret);
		if (cs == NULL) {
			cs = conn_state_create((unsigned long)ret);
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
