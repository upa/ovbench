sudo ip link add type gretap local 172.16.0.1 remote 172.16.0.2 
sudo ifconfig gretap1 up
sudo ifconfig gretap1 172.16.2.1/24
sudo arp -s 172.16.2.2 7a:a3:28:27:a3:aa
