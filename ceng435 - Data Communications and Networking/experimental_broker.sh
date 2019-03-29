#!/usr/bin/env bash

for INTERFACE in eth1 eth3
do
    ./experimental.sh $1 ${INTERFACE}
done
