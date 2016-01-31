/* fake.c
 * drop dummy driver 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/rculist.h>
#include <linux/hash.h>
#include <linux/udp.h>
#include <net/protocol.h>
#include <net/udp.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/ip6_route.h>
#include <net/udp_tunnel.h>
#include <net/vxlan.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>

#include "../modified-drivers/ovbench.h"


#define HDRROOM (sizeof (struct ethhdr) + sizeof (struct iphdr) + \
		 sizeof (struct udphdr))

#define HDRROOM_NOENCAP	HDRROOM
#define HDRROOM_VXLAN	HDRROOM + 8 + HDRROOM
#define HDRROOM_GRETAP	34 + 4 + HDRROOM
#define HDRROOM_GRE	HDRROOM_GRETAP - sizeof (struct ethhdr)
#define HDRROOM_IPIP	34 + HDRROOM - sizeof (struct ethhdr)
#define HDRROOM_NSH	HDRROOM + 8 + 24 + HDRROOM

#define HDRROOM_TIMESTAMP HDRROOM_IPIP


#define FAKE_VERSION	"0.0.0"
MODULE_VERSION (FAKE_VERSION);
MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("upa@haeena.net");
MODULE_DESCRIPTION ("drop dummy driver");
MODULE_ALIAS_RTNL_LINK ("fake");


static int fake_net_id;
static u32 fake_salt __read_mostly;


struct fake_dev {
	struct list_head	list;
	struct rcu_head		rcu;
	struct net_device	* dev;
};

struct fake_net {
	struct list_head	dev_list;	/* device list */
};



static netdev_tx_t
fake_xmit (struct sk_buff * skb, struct net_device * dev)
{
	uint64_t tsc;
	struct pcpu_sw_netstats * tx_stats;

	tsc = rdtsc ();

	if (OVTYPE_IS_IPIP (skb)) {

#if 0
		pr_info ("ovb ipip "
			 "ip_local_out->ipip_tunnel_xmit_in:%llu "
			 "ipip_tunnel_xmit_in->ip_tunnel_xmit_in:%llu "
			 "ip_tunnel_xmit_in->ip_tunnel_xmit_end:%llu "
			 "ip_tunnel_xmit_end->fake_xmit_in:%llu "
			 "all:%llu"
			 "\n",
			 ipip_ipip_tunnel_xmit_in (skb) - netdevgen_xmit (skb),
			 ip_tunnel_xmit_in (skb) - ipip_ipip_tunnel_xmit_in (skb),
			 ip_tunnel_xmit_end (skb) - ip_tunnel_xmit_in (skb),
			 tsc - ip_tunnel_xmit_end (skb),
			 tsc - netdevgen_xmit (skb)
			);
#endif

		pr_info ("ovb ipip "
			 "xmit-path-1st:%llu "
			 "ndo_start_xmit:%llu "
			 "routing-lookup:%llu "
			 "xmit-path-2nd:%llu "
			 "all:%llu"
			 "\n",
			 ipip_ipip_tunnel_xmit_in (skb) - netdevgen_xmit (skb),
			 ip_tunnel_xmit_in (skb) - ipip_ipip_tunnel_xmit_in (skb),
			 ip_tunnel_xmit_end (skb) - ip_tunnel_xmit_in (skb),
			 tsc - ip_tunnel_xmit_end (skb),
			 tsc - netdevgen_xmit (skb)
			);

	} else if (OVTYPE_IS_GRE (skb)) {

		pr_info ("ovb gre "
			 "xmit-path-1st:%llu "
			 "ndo_start_xmit:%llu "
			 "build-gre-header:%llu "
			 "routing-lookup:%llu "
			 "xmit-path-2nd:%llu "
			 "all:%llu "
			 "\n",
			 gre_ipgre_xmit_in (skb) - netdevgen_xmit (skb),
			 gre_gre_xmit_in (skb) - gre_ipgre_xmit_in (skb),
			 ip_tunnel_xmit_in (skb) - gre_gre_xmit_in (skb),
			 ip_tunnel_xmit_end (skb) - ip_tunnel_xmit_in (skb),
			 tsc - ip_tunnel_xmit_end (skb),
			 tsc - netdevgen_xmit (skb)
			);

	} else if (OVTYPE_IS_GRETAP (skb)) {

		pr_info ("ovb gretap "
			 "xmit-path-1st:%llu "
			 "ndo_start_xmit:%llu "
			 "build-gre-header:%llu "
			 "routing-lookup:%llu "
			 "xmit-path-2nd:%llu "
			 "all:%llu "
			 "\n",
			 gretap_gre_tap_xmit_in (skb) - netdevgen_xmit (skb),
			 gretap_gre_xmit_in (skb) - gretap_gre_tap_xmit_in (skb),
			 ip_tunnel_xmit_in (skb) - gre_gre_xmit_in (skb),
			 ip_tunnel_xmit_end (skb) - ip_tunnel_xmit_in (skb),
			 tsc - ip_tunnel_xmit_end (skb),
			 tsc - netdevgen_xmit (skb)
			);

	}  else if (OVTYPE_IS_VXLAN (skb)) {

#if 0
		pr_info ("ovb vxlan "
			 "ip_local_out->vxlan_xmit_in:%llu "
			 "vxlan_xmit_in->vxlan_xmit_one_in:%llu "
			 "vxlan_xmit_one_in->vxlan_xmit_skb_in:%llu "
			 "vxlan_xmit_skb_in->udp_tunnel_xmit_skb_in:%llu "
			 "udp_tunnel_xmit_skb_in->udp_tunnel_xmit_skb_end:%llu "
			 "udp_tunnel_xmit_skb_end->fake_xmit_in:%llu "
			 "all:%llu"
			 "\n",
			 vxlan_vxlan_xmit_in (skb) - netdevgen_xmit (skb),
			 vxlan_vxlan_xmit_one_in (skb) - vxlan_vxlan_xmit_in (skb),
			 vxlan_vxlan_xmit_skb_in (skb) - vxlan_vxlan_xmit_one_in (skb),
			 udp_tunnel_xmit_skb_in (skb) - vxlan_vxlan_xmit_skb_in (skb),
			 udp_tunnel_xmit_skb_end (skb) - udp_tunnel_xmit_skb_in (skb),
			 tsc - udp_tunnel_xmit_skb_end (skb),
			 tsc - netdevgen_xmit (skb)
			);
#endif
		pr_info ("ovb vxlan "
			 "xmit-path-1st:%llu "
			 "fdb-lookup:%llu "
			 "routing-lookup:%llu "
			 "build-vxlan-header:%llu "
			 "build-udp-header:%llu "
			 "xmit-path-2nd:%llu "
			 "all:%llu "
			 "\n",
			 vxlan_vxlan_xmit_in (skb) - netdevgen_xmit (skb),
			 vxlan_vxlan_xmit_one_in (skb) - vxlan_vxlan_xmit_in (skb),
			 vxlan_vxlan_xmit_skb_in (skb) - vxlan_vxlan_xmit_one_in (skb),
			 udp_tunnel_xmit_skb_in (skb) - vxlan_vxlan_xmit_skb_in (skb),
			 udp_tunnel_xmit_skb_end (skb) - udp_tunnel_xmit_skb_in (skb),
			 tsc - udp_tunnel_xmit_skb_end (skb),
			 tsc - netdevgen_xmit (skb)
			);

	} else if (OVTYPE_IS_NSH (skb)) {

#if 0
		pr_info ("ovb nsh "
			 "ip_local_out->nsh_xmit_in:%llu "
			 "nsh_xmit_in->nsh_xmit_lookup_end:%llu "
			 "nsh_xmit_lookup_end->nsh_xmit_vxlan_in:%llu "
			 "nsh_xmit_vxlan_in->nsh_xmit_vxlan_skb_in:%llu "
			 "nsh_xmit_vxlan_skb_in->udp_tunnel_xmit_skb_in:%llu "
			 "udp_tunnel_xmit_skb_in->udp_tunnel_xmit_skb_end:%llu "
			 "udp_tunnel_xmit_skb_end->fake_xmit_in:%llu "
			 "all:%llu"
			 "\n",
			 nsh_xmit_in (skb) - netdevgen_xmit (skb),
			 nsh_xmit_lookup_end (skb) - nsh_xmit_in (skb),
			 nsh_xmit_vxlan_in (skb) - nsh_xmit_lookup_end (skb),
			 nsh_xmit_vxlan_skb_in (skb) - nsh_xmit_vxlan_in (skb),
			 udp_tunnel_xmit_skb_in (skb) - nsh_xmit_vxlan_skb_in (skb),
			 udp_tunnel_xmit_skb_end (skb) - udp_tunnel_xmit_skb_in (skb),
			 tsc - udp_tunnel_xmit_skb_end (skb),
			 tsc - netdevgen_xmit (skb)
			);
#endif
		pr_info ("ovb nsh "
			 "xmit-path-1st:%llu "
			 "table-lookup:%llu "
			 "build-nsh-header:%llu "
			 "routing-lookup:%llu "
			 "build-vxlan-header:%llu "
			 "build-udp-header:%llu "
			 "xmit-path-2nd:%llu "
			 "all:%llu"
			 "\n",
			 nsh_xmit_in (skb) - netdevgen_xmit (skb),
			 nsh_xmit_lookup_end (skb) - nsh_xmit_in (skb),
			 nsh_xmit_vxlan_in (skb) - nsh_xmit_lookup_end (skb),
			 nsh_xmit_vxlan_skb_in (skb) - nsh_xmit_vxlan_in (skb),
			 udp_tunnel_xmit_skb_in (skb) - nsh_xmit_vxlan_skb_in (skb),
			 udp_tunnel_xmit_skb_end (skb) - udp_tunnel_xmit_skb_in (skb),
			 tsc - udp_tunnel_xmit_skb_end (skb),
			 tsc - netdevgen_xmit (skb)
			);

	} else if (OVTYPE_IS_NOENCAP (skb)) {

		pr_info ("ovb noencap "
			 "xmit-path:%llu"
			 "\n",
			 tsc - netdevgen_xmit (skb)
			);

	}
	tx_stats = this_cpu_ptr (dev->tstats);
	u64_stats_update_begin (&tx_stats->syncp);
	tx_stats->tx_packets++;
	tx_stats->tx_bytes += skb->len;
	u64_stats_update_end (&tx_stats->syncp);

	kfree_skb (skb);

	return NETDEV_TX_OK;
}

static int
fake_init (struct net_device * dev)
{
	dev->tstats = netdev_alloc_pcpu_stats (struct pcpu_sw_netstats);
	if (!dev->tstats)
		return -ENOMEM;

	return 0;
}

static void
fake_uninit (struct net_device * dev)
{
	free_percpu (dev->tstats);
	return;
}


static const struct net_device_ops fake_netdev_ops = {
	.ndo_init		= fake_init,
	.ndo_uninit		= fake_uninit,
	.ndo_start_xmit		= fake_xmit,
	.ndo_get_stats64	= ip_tunnel_get_stats64,	/* XXX */
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address	= eth_mac_addr,
};

static int
fake_newlink (struct net * net, struct net_device * dev,
	     struct nlattr * tb[], struct nlattr * data[])
{
	int err;
	struct fake_net * dnet = net_generic (net, fake_net_id);
	struct fake_dev * ddev = netdev_priv (dev);

	err = register_netdevice (dev);
	if (err) {
		netdev_err (dev, "failed to register netdevice\n");
		return err;
	}

	list_add_tail_rcu (&ddev->list, &dnet->dev_list);

	return 0;
}

static void
fake_dellink (struct net_device * dev, struct list_head * head)
{
	struct fake_dev * ddev = netdev_priv (dev);

	list_del_rcu (&ddev->list);

	unregister_netdevice_queue (dev, head);
	return;
}

static void
fake_setup (struct net_device * dev)
{
	struct fake_dev * ddev = netdev_priv (dev);

	eth_hw_addr_random (dev);
	ether_setup (dev);
	dev->netdev_ops = &fake_netdev_ops;
	dev->destructor = free_netdev;
	dev->tx_queue_len = 0;
	dev->features	|= NETIF_F_LLTX;
	dev->features  	|=  NETIF_F_NETNS_LOCAL;
	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE;
	netif_keep_dst (dev);

	INIT_LIST_HEAD (&ddev->list);
	ddev->dev = dev;

	return;
}

static struct rtnl_link_ops fake_link_ops __read_mostly = {
	.kind		= "fake",
	.priv_size	= sizeof (struct fake_dev),
	.setup		= fake_setup,
	.newlink	= fake_newlink,
	.dellink	= fake_dellink,
};

static __net_init int
fake_init_net (struct net * net)
{
	struct fake_net * dnet = net_generic (net, fake_net_id);

	INIT_LIST_HEAD (&dnet->dev_list);
	return 0;
}

static void __net_exit
fake_exit_net (struct net * net)
{
	struct fake_net * dnet = net_generic (net, fake_net_id);
	struct fake_dev * ddev, * next;
	LIST_HEAD (list);

	rtnl_lock ();
	list_for_each_entry_safe (ddev, next, &dnet->dev_list, list) {
		unregister_netdevice_queue (ddev->dev, &list);
	}
	rtnl_unlock ();

	return;
}

static struct pernet_operations fake_net_ops = {
	.init	= fake_init_net,
	.exit	= fake_exit_net,
	.id	= &fake_net_id,
	.size	= sizeof (struct fake_net),
};

static __init int
fake_init_module (void)
{
	int rc;

	get_random_bytes (&fake_salt, sizeof (fake_salt));

	rc = register_pernet_subsys (&fake_net_ops);
	if (rc)
		goto netns_failed;

	rc = rtnl_link_register (&fake_link_ops);
	if (rc)
		goto rtnl_failed;

	pr_info ("fake version %s loaded\n", FAKE_VERSION);

	return 0;

rtnl_failed:
	unregister_pernet_subsys (&fake_net_ops);
netns_failed:
	return rc;
}
module_init (fake_init_module);

static void __exit
fake_exit_module (void)
{
	rtnl_link_unregister (&fake_link_ops);
	unregister_pernet_subsys (&fake_net_ops);

	pr_info ("fake version %s unloaded\n", FAKE_VERSION);
}
module_exit (fake_exit_module);
