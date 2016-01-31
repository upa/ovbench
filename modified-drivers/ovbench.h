#ifndef _OVBENCH_H_
#define _OVBENCH_H_

#define BENCH_IPIP	1
#define BENCH_GRE	2
#define BENCH_VXLAN	3
#define BENCH_NSH	4


static inline __u64
rdtsc(void)
{
	__u32 lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return (__u64)hi << 32 | lo;
}



#define OVTYPE_IPIP	1
#define OVTYPE_GRE	2
#define OVTYPE_VXLAN	3
#define OVTYPE_NSH	4
#define OVTYPE_NOENCAP	5
#define OVTYPE_GRETAP	6


#define OVTYPE(skb) (skb->ovbench_type > 0)
#define OVTYPE_IS_IPIP(skb) (skb->ovbench_type == OVTYPE_IPIP)
#define OVTYPE_IS_GRE(skb) (skb->ovbench_type == OVTYPE_GRE)
#define OVTYPE_IS_VXLAN(skb) (skb->ovbench_type == OVTYPE_VXLAN)
#define OVTYPE_IS_NSH(skb) (skb->ovbench_type == OVTYPE_NSH)
#define OVTYPE_IS_NOENCAP(skb) (skb->ovbench_type == OVTYPE_NOENCAP)
#define OVTYPE_IS_GRETAP(skb) (skb->ovbench_type == OVTYPE_GRETAP)

#define ovbench_type(skb) (skb)->ovbench_type

#define netdevgen_xmit(skb) (skb)->ovbench_timestamp[3]
#define udp_tunnel_xmit_skb_in(skb) (skb)->ovbench_timestamp[4]
#define udp_tunnel_xmit_skb_end(skb) (skb)->ovbench_timestamp[5]
#define ip_tunnel_xmit_in(skb) (skb)->ovbench_timestamp[6]
#define ip_tunnel_xmit_end(skb) (skb)->ovbench_timestamp[7]


#define ipip_ipip_tunnel_xmit_in(skb) (skb)->ovbench_timestamp[0]
// ip_tunnel_xmit_in
// ip_tunnel_xmit_end

#define gre_ipgre_xmit_in(skb) (skb)->ovbench_timestamp[0]
#define gre_gre_xmit_in(skb) (skb)->ovbench_timestamp[1]
// ip_tunnel_xmit_in
// ip_tunnel_xmit_end

#define gretap_gre_tap_xmit_in(skb) (skb)->ovbench_timestamp[0]
#define gretap_gre_xmit_in(skb) (skb)->ovbench_timestamp[1]
// ip_tunnel_xmit_in
// ip_tunnel_xmit_end

#define vxlan_vxlan_xmit_in(skb) (skb)->ovbench_timestamp[0]
#define vxlan_vxlan_xmit_one_in(skb) (skb)->ovbench_timestamp[1]
#define vxlan_vxlan_xmit_skb_in(skb) (skb)->ovbench_timestamp[2]
// udp_tunnel_xmit_skb_in
// udp_tunnel_xmit_skb_end


#define nsh_xmit_in(skb) (skb)->ovbench_timestamp[0]
#define nsh_xmit_lookup_end(skb) (skb)->ovbench_timestamp[1]
#define nsh_xmit_vxlan_in(skb) (skb)->ovbench_timestamp[2]
#define nsh_xmit_vxlan_skb_in(skb) (skb)->ovbench_timestamp[6]
// udp_tunnel_xmit_skb_in
// udp_tunnel_xmit_skb_end



#endif
