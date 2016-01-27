
sudo ip tunnel add gre1 mode gre remote 172.16.0.2 local 172.16.0.1
sudo ifconfig gre1 up
sudo ifconfig gre1 172.16.3.1/24
