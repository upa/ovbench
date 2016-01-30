/*
 * netdev gen
 */


#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/atomic.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/net_namespace.h>

#include "../modified-drivers/ovbench.h"


MODULE_AUTHOR ("upa@haeena.net");
MODULE_DESCRIPTION ("netdevgen");
MODULE_LICENSE ("GPL");

static bool ndg_thread_running;
static struct task_struct * ndg_tsk, * ndg_one_tsk;

static int pktlen = 50;	// + ether 14-byte = 64-byte
//static __be32 srcip = 0x01010A0A; /* 10.10.1.1 */
//static __be32 dstip = 0x02010A0A; /* 10.10.1.2 */

#define NSH_MODE

#ifdef VXLAN_MODE
static __be32 srcip = 0x010110AC; /* 172.16.1.1 */
static __be32 dstip = 0x020110AC; /* 172.16.1.2 */
#define OVTYPE_MODE OVTYPE_VXLAN
#endif

#ifdef GRETAP_MODE
static __be32 srcip = 0x010210AC; /* 172.16.2.1 */
static __be32 dstip = 0x020210AC; /* 172.16.2.2 */
#endif

#ifdef GRE_MODE
static __be32 srcip = 0x010310AC; /* 172.16.3.1 */
static __be32 dstip = 0x020310AC; /* 172.16.3.2 */
#define OVTYPE_MODE OVTYPE_GRE
#endif

#ifdef IPIP_MODE
static __be32 srcip = 0x010510AC; /* 172.16.4.1 */
static __be32 dstip = 0x020510AC; /* 172.16.4.2 */
#define OVTYPE_MODE OVTYPE_IPIP
#endif

#ifdef NSH_MODE
static __be32 srcip = 0x010410AC; /* 172.16.4.1 */
static __be32 dstip = 0x020410AC; /* 172.16.4.2 */
#define OVTYPE_MODE OVTYPE_NSH
#endif

#ifdef NOENCAP_MODE
static __be32 srcip = 0x010010AC; /* 172.16.0.1 */
static __be32 dstip = 0x020010AC; /* 172.16.0.2 */
#define OVTYPE_MODE OVTYPE_NOENCAP
#endif


#define PROC_NAME "driver/netdevgen"

static atomic_t start;



static struct sk_buff *
netdevgen_build_packet (void)
{
	struct sk_buff * skb;
	struct iphdr * ip;
	struct udphdr * udp;
	struct flowi4 fl4;
	struct rtable * rt;
	struct net * net = get_net_ns_by_pid (1);

	if (!net) {
		pr_err ("failed to get netns by pid 1\n");
		return NULL;
	}

	/* alloc and build skb */
	skb = alloc_skb_fclone (2048, GFP_KERNEL);
	skb->protocol = htons (ETH_P_IP);
	skb_put (skb, pktlen);
	skb_set_network_header (skb, 0);
	skb_set_transport_header (skb, sizeof (*ip));

	ip = (struct iphdr *) skb_network_header (skb);
	ip->ihl		= 5;
	ip->version	= 4;
	ip->tos		= 0;
	ip->tot_len	= htons (pktlen);
	ip->id		= 0;
	ip->frag_off	= 0;
	ip->ttl		= 12;
	ip->protocol	= IPPROTO_UDP;	
	ip->check	= 0;
	ip->saddr	= srcip;
	ip->daddr	= dstip;

	udp = (struct udphdr *) skb_transport_header (skb);
	udp->check	= 0;
	udp->source	= 0;
	udp->dest	= 0;
	udp->len	= htons (pktlen - sizeof (*ip));

	memset (&fl4, 0, sizeof (fl4));
	fl4.saddr = srcip;
	fl4.daddr = dstip;
	rt = ip_route_output_key (net, &fl4);
	if (IS_ERR (rt)) {
		pr_err ("no route to %pI4 from %pI4\n", &dstip, &srcip);
		return NULL;
	}
	skb_dst_drop (skb);
	skb_dst_set (skb, &rt->dst);
	
	skb->ovbench_type = OVTYPE_MODE;

	return skb;
}

static void
netdevgen_xmit_one (void)
{
	struct sk_buff * skb;

	skb = netdevgen_build_packet ();
	if (!skb) {
		pr_err ("skb build failed\n");
		return;
	}

	netdevgen_xmit (skb) = rdtsc ();

	ip_local_out (skb);
}

static int
netdevgen_xmit_one_thread (void * arg)
{
	netdevgen_xmit_one ();

	return 0;
}

static int
netdevgen_thread (void * arg)
{
	struct sk_buff * skb, * pskb;

	ndg_thread_running = true;

	skb = netdevgen_build_packet ();
	if (!skb) {
		pr_err ("skb build failed\n");
		goto err_out;
	}

	while (!kthread_should_stop ()) {

		if (atomic_read (&start) == 0) {
			break;
		}

		pskb = skb_clone (skb, GFP_KERNEL);
		if (!pskb) {
			pr_err ("failed to clone skb\n");
			continue;
		}

		ip_local_out (pskb);
	}

err_out:
	//kfree_skb (skb);
	ndg_thread_running = false;

	pr_info ("netdevgen thread finished\n");

	return 0;
}


static void
start_netdevgen_thread (void)
{
	if (!atomic_read (&start)) {
		pr_info ("netdevgen: thread start\n");
		atomic_set (&start, 1);
		ndg_tsk = kthread_run (netdevgen_thread, NULL, "netdevgen");
	}
}

static void
stop_netdevgen_thread (void)
{
	if (!atomic_read (&start)) {
		pr_info ("netdevgen: thread stop\n");
		atomic_set (&start, 0);
	}
}

static void
start_netdevgen_xmit_one_thread (void)
{
	ndg_one_tsk = kthread_run (netdevgen_xmit_one_thread,
				   NULL, "netdevgen");
}

static void
start_stop (void)
{
	if (atomic_read (&start)) {
		pr_info ("netdevgen: start -> stop\n");
		stop_netdevgen_thread ();
	} else {
		pr_info ("netdevgen: stop -> start\n");
		start_netdevgen_thread ();
	}
}

static ssize_t
proc_read(struct file *fp, char *buf, size_t size, loff_t *off)
{
	pr_info ("proc read\n");

	//copy_to_user (buf, "stop!\n", size);
	start_stop ();
	return size;
}

static ssize_t
proc_write(struct file *fp, const char *buf, size_t size, loff_t *off)
{
	if (strncmp (buf, "xmit", 4) == 0) {
		start_netdevgen_xmit_one_thread ();
	} else if (strncmp (buf, "start", 5) == 0) {
		start_netdevgen_thread ();
	} else if (strncmp (buf, "stop", 4) == 0) {
		stop_netdevgen_thread ();
	} else {
		pr_info ("invalid command\n");
	}

        return size;
}

static const struct file_operations proc_file_fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};

static int __init
netdevgen_init (void)
{
	struct proc_dir_entry * ent;

        ent = proc_create(PROC_NAME, S_IRUGO | S_IWUGO | S_IXUGO,
			  NULL, &proc_file_fops);
        if (ent == NULL)
                return -ENOMEM;


	atomic_set (&start, 0);

	if (IS_ERR (ndg_tsk)) {
		pr_err ("failed to run netdevgen thread\n");
		return -1;
	}

	pr_info ("netdevgen loaded\n");
		
	return 0;
}

static void __exit
netdevgen_exit (void)
{
	remove_proc_entry (PROC_NAME, NULL);

	if (ndg_tsk && ndg_thread_running)
		kthread_stop (ndg_tsk);
	else {
		pr_info ("thread is already done\n");
	}

	pr_info ("netdevgen unloaded\n");

	return;
}

module_init (netdevgen_init);
module_exit (netdevgen_exit);
