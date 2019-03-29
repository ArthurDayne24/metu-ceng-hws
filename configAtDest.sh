#!/usr/bin/env bash
# config router 1
route add -net 10.10.2.1 netmask 255.255.255.255 gw 10.10.3.1 eth1
# config router 2
route add -net 10.10.4.1 netmask 255.255.255.255 gw 10.10.5.1 eth2
