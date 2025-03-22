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
		return NF_DROP;
	}
	if (ip->protocol == IPPROTO_TCP) {
		tcp = INCOMING_TCPH(ip);
		len = ntohs(ip->tot_len) - (tcp->doff * 4) - (ip->ihl * 4);
		printk(KERN_ALERT "TCP Packet received\n");
		data = (unsigned char*)tcp + tcp->doff * 4;
		printk(KERN_ALERT "doff is %d\n", tcp->doff);
		if (strncmp(data, "hello", 5) == 0) {
			for (i = 0; i < len; i++) {
				if (data[i] == '\n' || data[i] == '\r') continue;
				data[i] = 'A' + (data[i] - 'a');
			}
		}
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops hook_ops = {
	.hook        = hook,
	.hooknum     = NF_INET_LOCAL_IN,
	.pf          = NFPROTO_IPV4,
	.priority    = NF_IP_PRI_FIRST
};

static int __init mod_start(void) {
	return nf_register_net_hook(&init_net, &hook_ops);
}

static void __exit mod_stop(void) {
	nf_unregister_net_hook(&init_net, &hook_ops);
	printk(KERN_ALERT "Goodbye, World\n");
	
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("Netfilter Lab Solution");
module_init(mod_start);
module_exit(mod_stop); 
