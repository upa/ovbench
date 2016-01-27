
sudo ip link add type nsh spi 10 si 5
sudo ifconfig nsh0 up
sudo ifconfig nsh0 172.16.4.1/24
sudo arp -s 172.16.4.2 7a:a3:28:27:a3:ab

sudo ip nsh add spi 10 si 5 encap vxlan remote 172.16.0.2 local 172.16.0.1 vni 0
