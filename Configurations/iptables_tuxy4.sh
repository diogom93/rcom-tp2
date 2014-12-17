#!/bin/bash

iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE
iptables -A FORWARD -i eth1 -m state --state NEW,INVALID -j DROP
iptables -L
iptabes -t nat -L