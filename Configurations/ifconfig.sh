#!/bin/bash

echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

ifconfig eth0 172.16.20.254/24
ifconfig eth0 172.16.21.253/24

route add default gw 172.16.21.254/24