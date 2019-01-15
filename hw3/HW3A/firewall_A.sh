#!/usr/bin/env bash

HOSTB=192.168.43.70

# disable firewall
if [[ $1 == "-d" ]];
then
    ufw disable
    ufw reset
    ufw disable

    exit
fi

# enable firewall
if [[ $1 == "-e" ]];
then
    ufw reset
    ufw enable

    # for icmp packets
    cp etc.ufw.before.rules /etc/ufw/before.rules
    # for facebook
    ufw deny out from any to 157.240.9.35
    # for telnet
    # B -> A
    ufw deny proto tcp from ${HOSTB} to any
    # A -> B
    ufw deny out proto tcp from any to ${HOSTB}
    # for https (443)
    ufw deny out from any to any port 443

    ufw reload

    exit
fi

echo Nothing is done, parameter should be -d or -e to disable/enable
