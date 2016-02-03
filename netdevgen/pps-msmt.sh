#!/bin/sh

pktlen=1508

sudo rmmod netdevgen

for pktlen in 46 110 238 494 1006 1482 1500; do

	sudo insmod ./netdevgen.ko

	for t in vxlan gretap gre ipip nsh noencap; do
		echo xmit $t pakcet 
		echo $t > /proc/driver/netdevgen

		sleep 5

		for n in `seq 120`; do
			ifdata -pops fake0 >> pps-result/result-$pktlen-$t.txt
		done

		echo stop > /proc/driver/netdevgen
		sleep 1
		sudo rmmod netdevgen
	done
done
