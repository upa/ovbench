
sudo ip tunnel add ipip1 mode ipip remote 172.16.0.2 local 172.16.0.1
sudo ifconfig ipip1 up
sudo ifconfig ipip1 172.16.5.1/24
