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


modprobe libertas_sdio

#   shutdown other ports
ifconfig usb0   down 2>/dev/null
ifconfig eth0   down 2>/dev/null
ifconfig usb1   down 2>/dev/null

ifconfig eth1 up
iwconfig eth1 mode managed  essid armadeus key restricted 93d7-e853-22e8-8784-0b4a-629c-ab 


ifconfig eth1 192.168.0.251
sleep 1
echo -e "\nTo check the wifi port, please browse on 192.168.0.251\n"
