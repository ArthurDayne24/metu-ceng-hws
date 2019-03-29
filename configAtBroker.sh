#!/usr/bin/env bash
# config router 1
route add -net 10.10.3.2 netmask 255.255.255.255 gw 10.10.2.2 eth3
# config router 2
route add -net 10.10.5.2 netmask 255.255.255.255 gw 10.10.4.2 eth1
