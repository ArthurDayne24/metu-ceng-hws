#!/usr/bin/env bash

# base script to set experimental options in nodes

if [[ "$1" == "clear" ]]; then
    echo Clear netem/tc settings for interface $2
    tc qdisc replace dev $2 root netem loss 0% corrupt 0% duplicate 0% delay 0ms reorder 0% 0%
    tc qdisc del dev $2 root netem
    exit
fi


if [[ "$1" == "1.1" ]]; then
    echo Config $2 for experiment 1.1
    tc qdisc replace dev $2 root netem loss 0.5% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi
if [[ "$1" == "1.2" ]]; then
    echo Config $2 for experiment 1.2
    tc qdisc replace dev $2 root netem loss 10% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi
if [[ "$1" == "1.3" ]]; then
    echo Config $2 for experiment 1.3
    tc qdisc replace dev $2 root netem loss 20% corrupt 0% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi


if [[ "$1" == "2.1" ]]; then
    echo Config $2 for experiment 2.1
    tc qdisc replace dev $2 root netem loss 0% corrupt 0.2% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi
if [[ "$1" == "2.2" ]]; then
    echo Config $2 for experiment 2.2
    tc qdisc replace dev $2 root netem loss 0% corrupt 10% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi
if [[ "$1" == "2.3" ]]; then
    echo Config $2 for experiment 2.3
    tc qdisc replace dev $2 root netem loss 0% corrupt 20% duplicate 0% delay 3ms reorder 0% 0%
    exit
fi


if [[ "$1" == "3.1" ]]; then
    echo Config $2 for experiment 3.1
    tc qdisc replace dev $2 root netem loss 0% corrupt 0% duplicate 0% delay 3ms reorder 1% 50%
    exit
fi
if [[ "$1" == "3.2" ]]; then
    echo Config $2 for experiment 3.2
    tc qdisc replace dev $2 root netem loss 0% corrupt 0% duplicate 0% delay 3ms reorder 10% 50%
    exit
fi
if [[ "$1" == "3.3" ]]; then
    echo Config $2 for experiment 3.3
    tc qdisc replace dev $2 root netem loss 0% corrupt 0% duplicate 0% delay 3ms reorder 35% 50%
    exit
fi

echo Missing parameter \<EXPERIMENT\>
