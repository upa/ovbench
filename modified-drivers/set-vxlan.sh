sudo ip link add type vxlan local 172.16.0.1 remote 172.16.0.2 id 0
sudo ifconfig vxlan0 up
sudo ifconfig vxlan0 172.16.1.1/24
sudo arp -s 172.16.1.2 7a:a3:28:27:a3:a9
