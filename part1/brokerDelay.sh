sudo tc qdisc replace dev eth2 root netem delay 1ms 5ms distribution normal
sudo tc qdisc replace dev eth3 root netem delay 1ms 5ms distribution normal
