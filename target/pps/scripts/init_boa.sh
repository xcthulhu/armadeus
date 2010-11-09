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

# setup /etc/hosts file (hosname id required by "boa")
echo "127.0.0.1       localhost" > /etc/hosts
echo "192.168.0.250   armadeus"  >> /etc/hosts

if [ -d "/var/www" ]; then
	kill -9 `pidof boa` 2> /dev/null
	mkdir /var/log/boa 2>/dev/null 
	boa
fi

