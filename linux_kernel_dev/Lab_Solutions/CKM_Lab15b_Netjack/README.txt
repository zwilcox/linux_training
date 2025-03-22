accept: 
at the moment the syscall is called, current will be the task_struct of the programming calling accept. If it's ncat then we add it to our list of tracked sockets.

recv:
The idea of iovecs is not new, it was actually mentioned briefly in linux sys programming when talking about rcvmsg/sendmsg ~slide 131.
The man pages for recvmsg show the struct msghdr and iovec:
struct iovec { /* Scatter/gather array items */
	void *iov_base; /* Starting address */
	size_t iov_len; /* Number of bytes to transfer */
};

struct msghrd {
	...
	struct iovec *msg_iov; /* scatter/gather array */
	...
}
As we will see in a moment, both send/recv and sendmsg/rcvmsg will eventually get to the recvmsg call.

The main thing to find is where in the kernel does a recv populate this msg->msg_iter.iov->iov_base so we know where we should modify input before calling original and modify output once it has been copied in.

Let's start at the syscall entry point for recv.

In /net/socket.c 2040, recv just calls __sys_recvfrom()
SYSCALL_DEFINE4(recv, int, fd, void __user *, ubuf, size_t, size,
		unsigned int, flags)
{
			return __sys_recvfrom(fd, ubuf, size, flags, NULL, NULL);
}

/net/socket.c 1988
https://elixir.bootlin.com/linux/v5.2.11/source/net/socket.c#L1988
a struct iovec iov and struct msghdr msg are initialized and passed into import_single_range() along with the user buffer and size of buffer.

__sys_recvfrom(int fd, void __user *ubuf, size_t size, unsigned int flags,
				struct sockaddr __user *addr, int __user *addr_len)
{
		struct socket *sock;
		struct iovec iov;
		struct msghdr msg;
		int err, err2;
		...

		err = import_single_range(READ, ubuf, size, &iov, &msg.msg_iter);
		...
}

/lib/iov_iter.c 1682
https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L1682

import_single_range() initializes the iovec to point to the user buffer provided, and sets the size to be the size provided. These are the buffer and size the user gave when they made the recv call. Then it calls iov_iter_init().

int import_single_range(int rw, void __user *buf, size_t len,
		 struct iovec *iov, struct iov_iter *i)
{
	...
	iov->iov_base = buf;
	iov->iov_len = len;
	iov_iter_init(i, rw, iov, 1, len);
	return 0;
}

iov_iter_init() takes in i, which is &msg.msg_iter, and iov which is the iovec that was initialized.
iov_iter_init() then sets i to be iov. There's also a check to see if we are in user or kernel mode with uaccess_kernel(). If it was kernel mode then type is set to ITER_KVEC, if user it's set to ITER_IOVEC.

https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L436
void iov_iter_init(struct iov_iter *i, unsigned int direction,
			const struct iovec *iov, unsigned long nr_segs,
			size_t count)
{
	WARN_ON(direction & ~(READ | WRITE));
	direction &= READ | WRITE;

	/* It will get better.  Eventually... */
	if (uaccess_kernel()) {
		i->type = ITER_KVEC | direction;
		i->kvec = (struct kvec *)iov;
	} else {
		i->type = ITER_IOVEC | direction;
		i->iov = iov;
	}
	i->nr_segs = nr_segs;
	i->iov_offset = 0;
	i->count = count;
}

That might be enough to see that msg.msg_iter is the area where we need to look once rcv is done, which is what the solution does. After this, if we continue with the code we can see the area where the sk_buff received data gets copied to this msg struct.

So returning to __sys_recvmsg, after import_single_range, msg.msg_iter is pointing to a struct iovec that includes the user provided buffer and size.

Eventually __sys_recvmsg will call sock_recvmsg(sock, &msg, flags) /net/socket.c 2015

sock_recvmsg has a security_* check which will give a chance for any security module to check params etc. This common pattern of security_* followed by *_nosec() may have been mentioned in previous labs/slides. 

Then it moves on to sock_recvmsg_nosec() if it's allowed to call recv. Note that the recvmsg() syscall will also eventually get to this sock_recvmsg call if you followed from the SYSCALL_DEFINE of recvmsg instead of recv().

sock_recvmsg_nosec will call inet_recvmsg, which eventually calls the recvmsg for the specific protocol sk->sk_prot->recvmsg() (see networks slides from kernel internals). We are going to assume TCP, so we will look at tcp_rcvmsg.

tcp_recvmsg does a lot of state checks etc, then eventually it gets to the main loop. There it will get an sk_buff from the receive queue, if it gets one that fits the checks then it will go to the label found_ok_skb which will copy the data into msg using skb_copy_datagram_msg.

https://elixir.bootlin.com/linux/v5.2.11/source/net/ipv4/tcp.c#L1934
int tcp_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int nonblock,
		int flags, int *addr_len)
{
	...
	do {
		...
		/* Next get a buffer. */

		last = skb_peek_tail(&sk->sk_receive_queue);
		skb_queue_walk(&sk->sk_receive_queue, skb) {
			last = skb;
			/* Now that we have two receive queues this
			 * shouldn't happen.
			 */
			...
			if (offset < skb->len)
				goto found_ok_skb;

			...

found_ok_skb:
		/* Ok so how much can we use? */
		...
		if (!(flags & MSG_TRUNC)) {
			err = skb_copy_datagram_msg(skb, offset, msg, used);

skb_copy_datagram_msg is just a wrapper around skb_copy_datagram_iter, which then just calls __skb_datagram_iter(), passing in the sk_buff to copy from and the iov_iter to copy to.

https://elixir.bootlin.com/linux/v5.2.11/source/net/core/datagram.c#L525
/**
 *	skb_copy_datagram_iter - Copy a datagram to an iovec iterator.
 *	@skb: buffer to copy
 *	@offset: offset in the buffer to start copying from
 *	@to: iovec iterator to copy to
 *	@len: amount of data to copy from buffer to iovec
 */
int skb_copy_datagram_iter(const struct sk_buff *skb, int offset,
			   struct iov_iter *to, int len)
{
	trace_skb_copy_datagram_iovec(skb, len);
	return __skb_datagram_iter(skb, offset, to, len, false,
			simple_copy_to_iter, NULL);
}

__skb_datagram_iter will copy using the function pointer cb that is passed in to it. In this case it's simple_copy_to_iter.
https://elixir.bootlin.com/linux/v5.2.11/source/net/core/datagram.c#L410
static int __skb_datagram_iter(const struct sk_buff *skb, int offset,
			       struct iov_iter *to, int len, bool fault_short,
			       size_t (*cb)(const void *, size_t, void *,
					    struct iov_iter *), void *data)
{
	int start = skb_headlen(skb);
	int i, copy = start - offset, start_off = offset, n;
	struct sk_buff *frag_iter;

	/* Copy header. */
	if (copy > 0) {
		if (copy > len)
			copy = len;
		n = cb(skb->data + offset, copy, data, to);
		...
	}

	/* Copy paged appendix. Hmm... why does this look so complicated? */
	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
			...
			n = cb(vaddr + frag->page_offset +
				offset - start, copy, data, to);
			...
	}


	skb_walk_frags(skb, frag_iter) {
			...
			if (__skb_datagram_iter(frag_iter, offset - start,
						to, copy, fault_short, cb, data))
			...
	}
	...
}

simple_copy_to_iter is a wrapper around copy_to_iter, which after an input check calls _copy_to_iter.
https://elixir.bootlin.com/linux/v5.2.11/source/net/core/datagram.c#L512 simple_copy_to_iter
https://elixir.bootlin.com/linux/v5.2.11/source/include/linux/uio.h#L141 copy_to_iter


_copy_to_iter takes in the sk_buff's addr, number of bytes to copy, and the iov_iter. Earlier in iov_iter_init, if it was a user call then the type is set to ITER_IOVEC, if from kernel then it's set to ITER_KVEC. iov_iter_is_pipe() will check for the ITER_PIPE type, so since it's not that it will go to iterate_and_advance.

https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L603
size_t _copy_to_iter(const void *addr, size_t bytes, struct iov_iter *i)
{
	const char *from = addr;
	if (unlikely(iov_iter_is_pipe(i)))
		return copy_pipe_to_iter(addr, bytes, i);
	if (iter_is_iovec(i))
		might_fault();
	iterate_and_advance(i, bytes, v,
		copyout(v.iov_base, (from += v.iov_len) - v.iov_len, v.iov_len),
		memcpy_to_page(v.bv_page, v.bv_offset,
			       (from += v.bv_len) - v.bv_len, v.bv_len),
		memcpy(v.iov_base, (from += v.iov_len) - v.iov_len, v.iov_len)
	)

	return bytes;
}

iterate_and_advance is a macro that's hard to read since it uses only one-letter variables. But it will go through each of the remaining types and call the iterate_*() function that corresponds to the type used. iterate_kvec will take in K, which is memcpy(), and the default iterate_iovec will take in I which is copyout().

https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L97
#define iterate_and_advance(i, n, v, I, B, K) {			\
	...
	if (i->count) {						\
		...
		} else if (unlikely(i->type & ITER_KVEC)) {	\
			const struct kvec *kvec;		\
			struct kvec v;				\
			iterate_kvec(i, n, v, kvec, skip, (K))	\
			if (skip == kvec->iov_len) {		\
				kvec++;				\
				skip = 0;			\
			}					\
			i->nr_segs -= kvec - i->kvec;		\
			i->kvec = kvec;				\
		} else if (unlikely(i->type & ITER_DISCARD)) {	\
			skip += n;				\
		} else {					\
			const struct iovec *iov;		\
			struct iovec v;				\
			iterate_iovec(i, n, v, iov, skip, (I))	\
			if (skip == iov->iov_len) {		\
				iov++;				\
				skip = 0;			\
			}					\
			i->nr_segs -= iov - i->iov;		\
			i->iov = iov;				\
		}						\
		i->count -= n;					\
		i->iov_offset = skip;				\
	}							\
}

https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L14
STEP is when it calls copyout() in the left = (STEP) 
#define iterate_iovec(i, n, __v, __p, skip, STEP) {	\
	size_t left;					\
	size_t wanted = n;				\
	__p = i->iov;					\
	__v.iov_len = min(n, __p->iov_len - skip);	\
	if (likely(__v.iov_len)) {			\
		__v.iov_base = __p->iov_base + skip;	\
		left = (STEP);				\                 
		__v.iov_len -= left;			\
		skip += __v.iov_len;			\
		n -= __v.iov_len;			\
	} else {					\
		left = 0;				\
	}						\
	while (unlikely(!left && n)) {			\
		__p++;					\
		__v.iov_len = min(n, __p->iov_len);	\
		if (unlikely(!__v.iov_len))		\
			continue;			\
		__v.iov_base = __p->iov_base;		\
		left = (STEP);				\
		__v.iov_len -= left;			\
		skip = __v.iov_len;			\
		n -= __v.iov_len;			\
	}						\
	n = wanted - n;					\
}

copyout() after some checks is our familiar copy_to_user. So that's how we copy from sk_buff to the msg->msg_iter.iov which is the user buffer.

https://elixir.bootlin.com/linux/v5.2.11/source/lib/iov_iter.c#L138
static int copyout(void __user *to, const void *from, size_t n)
{
	if (access_ok(to, n)) {
		kasan_check_read(from, n);
		n = raw_copy_to_user(to, from, n);
	}
	return n;
}


