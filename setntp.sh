#!/usr/bin/env bash
sudo /etc/init.d/ntp stop
sudo /usr/sbin/ntpdate pool.ntp.org
