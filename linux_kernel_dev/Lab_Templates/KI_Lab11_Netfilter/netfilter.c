#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>

// For TCP packets (iph->protocol == IPPROTO_TCP)
#define INCOMING_TCPH(x)	((struct tcphdr*)((__u32*)x + x->ihl))
// For UDP packets (iph->protocol == IPPROTO_UDP)
#define INCOMING UDPH(x)	((struct udphdr*)((__u32*)x + x->ihl))

static unsigned int hook(void* priv, struct sk_buff* skb, const struct nf_hook_state* state);

unsigned int hook(void* priv, struct sk_buff* skb, const struct nf_hook_state* state) {
	struct iphdr* ip;
	struct tcphdr* tcp;
	unsigned char* data;
	unsigned int len;
	unsigned int i;

	if (skb == NULL) {
		return NF_ACCEPT;
	}

	ip = ip_hdr(skb);
	if (ip->protocol == IPPROTO_ICMP) {
		return /* TODO: what should we do with ICMP packets? */;
	}
	if (ip->protocol == IPPROTO_TCP) {
		// TODO: receive some tcp packets
		printk(KERN_ALERT "TCP Packet received\n");
		// TODO: offset to the data and change 'hello' to 'HELLO'
	}

	return /* TODO: what should we do with TCP packets? */;
}

static struct nf_hook_ops hook_ops = {
	.hook        = hook,
	.hooknum     = /* TODO: what hook location do we want? */,
	.pf          = /* TODO: what family do we want? */,
	.priority    = /* TODO: what priority do we want? */
};

static int __init mod_start(void) {
	return /* TODO: register our hook */;
}

static void __exit mod_stop(void) {
	/* TODO: unregister our hook */;
	printk(KERN_ALERT "Goodbye, World\n");
	
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Netfilter Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 
