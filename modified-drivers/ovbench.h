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

#define OVTYPE(skb) (skb->ov_type > 0)
#define OVTYPE_IS_IPIP(skb) (skb->ov_type == OVTYPE_IPIP)
#define OVTYPE_IS_GRE(skb) (skb->ov_type == OVTYPE_GRE)
#define OVTYPE_IS_VXLAN(skb) (skb->ov_type == OVTYPE_VXLAN)
#define OVTYPE_IS_NSH(skb) (skb->ov_type == OVTYPE_NSH)


#define ov_type(skb) (skb)->ov_type

#define udp_tunnel_xmit_skb_in(skb) (skb)->ov_timestamp[4]
#define udp_tunnel_xmit_skb_end(skb) (skb)->ov_timestamp[5]
#define ip_tunnel_xmit_in(skb) (skb)->ov_timestamp[6]
#define ip_tunnel_xmit_end(skb) (skb)->ov_timestamp[7]


#define ipip_ipip_tunnel_xmit_in(skb) (skb)->ov_timestamp[0]
// ip_tunnel_xmit_in
// ip_tunnel_xmit_end

#define gre_ipgre_xmit_in(skb) (skb)->ov_timestamp[0]
#define gre_ipgre_xmit_int(skb) (skb)->ov_timestamp[1]
// ip_tunnel_xmit_in
// ip_tunnel_xmit_end

#define vxlan_vxlan_xmit_in(skb) (skb)->ov_timestamp[0]
#define vxlan_vxlan_xmit_one_in(skb) (skb)->ov_timestamp[1]
#define vxlan_vxlan_xmit_skb_in(skb) (skb)->ov_timestamp[2]
// udp_tunnel_xmit_skb_in
// udp_tunnel_xmit_skb_end


#define nsh_xmit_in(skb) (skb)->ov_timestamp[0]
#define nsh_xmit_vxlan_in(skb) (skb)->ov_timestamp[1]
// udp_tunnel_xmit_skb_in
// udp_tunnel_xmit_skb_end


#endif
