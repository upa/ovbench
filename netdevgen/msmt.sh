#!/bin/sh


for t in vxlan gretap gre ipip nsh noencap; do
	for n in `seq 1 100`; do 
		echo xmit $t pakcet $n
		echo $t > /proc/driver/netdevgen
		sleep 1
	done
done
