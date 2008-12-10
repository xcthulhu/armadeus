#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2008 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
source ./test_env.sh

test_usb_gadget()
{
	show_test_banner "USB gadget"
	BACKING_FILE="/tmp/backing_file.gz"
	dmesg | grep "imx_udc version"
	if [ "$?" == 0 ]; then
		udhcpc -i eth0
		echo "Downloading test file from Internet"
		wget http://dl.free.fr/p9VCjtQuc -O /tmp/backing_file.gz
		#tftp -g -r backing_file.gz -l /tmp/backing_file.gz $SERVER_IP
		gunzip /tmp/backing_file.gz
		modprobe g_file_storage file=/tmp/backing_file
		sleep 1
		ask_user "Please connect your board to your PC and see if a mass storage\ndevice is detected. When done, unmount it and press <ENTER>"
	else
		echo "UDC core not launched"
	fi
}

test_usb_gadget