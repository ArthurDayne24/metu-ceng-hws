#!/usr/bin/env bash

# -e/-d(enable/disable firewall) (other host's ip) (website's ip)
# last two arguments are needed if first argument is -e

# disable firewall
if [[ $1 == "-d" ]];
then
    ufw disable
    ufw reset
    ufw disable

    ufw reload
    
    exit
fi

HOSTB=10.0.2.5
WEBSITEIP=157.240.9.35

if [[ "$#" -eq 3 ]]; then
    WEBSITEIP=$3
    HOSTB=$2
fi

if [[ "$#" -eq 2 ]]; then
    HOSTB=$2
fi

# enable firewall
if [[ $1 == "-e" ]];
then
    ufw reset
    ufw enable

    # for icmp packets
    cp etc.ufw.before.rules /etc/ufw/before.rules
    # for facebook
    ufw deny out from 10.0.2.4 to ${WEBSITEIP}
    # for telnet
    # B -> A
    ufw deny proto tcp from ${HOSTB} to 10.0.2.4 port 23
    # A -> B
    ufw deny out from 10.0.2.4 to ${HOSTB} port 23
    # for https (443)
    ufw deny out from 10.0.2.4 to any port 443

    ufw reload

    exit
fi

echo Nothing is done, parameter should be -d or -e to disable/enable
