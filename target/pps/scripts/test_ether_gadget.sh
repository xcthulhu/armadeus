#
#        This software has been developed by ARMADEUS SYSTEMS
#
# Customer Name: HTSC
#
# Project Name: PPS 
#
# Copyright (C) 2010 : 
# 
# This program is the property of the armadeus systems company or of the 
# final customer as soon as terms & conditions stipulated by the contract
# between the two parties are full-filled .
#
# 	Written by: Burkhart Frederic,,,
#

modprobe -r smsc95xx
modprobe -r g_ether

modprobe g_ether

#   shutdown other ports
ifconfig usb0   down 2>/dev/null
ifconfig eth0   down 2>/dev/null
ifconfig eth1   down 2>/dev/null

ifconfig usb0  192.168.10.1

echo -e "\nPour verifier le port usb gadget (usb0) , veuiller browser sur 192.168.10.1\n"

