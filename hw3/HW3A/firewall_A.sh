#!/usr/bin/env bash

# disable firewall
if [[ $1 == "-d" ]];
then
    ufw reset

    ufw disable
    exit
fi

# enable firewall
if [[ $1 == "-e" ]];
then
    ufw enable
    
    # facebook
    ufw reject from 157.240.9.35 to any
    # telnet
    ufw reject from 192.168.43.5 to any port 23
    # icmp
    ufw reject 7
    # https
    ufw reject https

    exit
fi

echo Nothing is done, parameter should be -d or -e to disable/enable
