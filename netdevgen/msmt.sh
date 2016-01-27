#!/bin/sh


for n in `seq 1 100`;do 
	echo xmit pakcet $n
	echo xmit > /proc/driver/netdevgen
	sleep 0.5
done
