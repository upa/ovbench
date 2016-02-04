#!/bin/sh

pktlen=1508

sudo rmmod netdevgen



#for pktlen in 114 242 498 1012 1486; do
for pktlen in 1010; do

	sudo insmod ./netdevgen.ko pktlen="$pktlen"
	sleep 5

	for t in vxlan gretap gre ipip nsh noencap; do

		echo xmit $t pakcet, pktlen is $pktlen
		echo $t > /proc/driver/netdevgen

		sleep 5
		fpktlen=`expr $pktlen + 14`

		for n in `seq 120`; do
			ifdata -pops fake0 >> pps-result/result-$fpktlen-$t.txt
		done

		echo stop > /proc/driver/netdevgen
		sleep 1
	done

	sudo rmmod netdevgen
done
