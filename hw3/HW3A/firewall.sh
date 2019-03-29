#!/usr/bin/env bash

# -e/-d(enable/disable firewall) (own ip) (other host's ip) (website's ip)
# last three arguments are needed if first argument is -e

# disable firewall
if [[ $1 == "-d" ]];
then
    ufw disable
    ufw reset
    ufw disable

    ufw reload
    
    exit
fi

# enable firewall
if [[ $1 == "-e" ]] && [[ "$#" -eq 4 ]];
then
    WEBSITEIP=$4
    HOSTB=$3
    HOSTA=$2 # own

    ufw reset
    ufw enable

    # for icmp packets
    cp etc.ufw.before.rules /etc/ufw/before.rules
    # for metu.edu.tr
    ufw deny out from ${HOSTA} to ${WEBSITEIP}
    ## for telnet
    ## B -> A
    ufw deny proto tcp from ${HOSTB} to ${HOSTA} port 23
    ## A -> B
    ufw deny out from ${HOSTA} to ${HOSTB} port 23
    ## for https (443)
    ufw deny out from ${HOSTA} to any port 443

    ufw reload

    exit
fi

echo Nothing is done, parameter should be -d or -e to disable/enable
