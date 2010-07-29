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
	dmesg | grep "imx_udc"
	if [ "$?" == 0 ]; then
		udhcpc -i eth0
		echo "Downloading test file from Internet"
		wget http://dl.free.fr/q9IKqqweK -O $BACKING_FILE
		if [ "$?" != 0 ]; then
			exit_failed
		fi
		gunzip $BACKING_FILE
		modprobe g_file_storage file=/tmp/backing_file
		sleep 1
		ask_user "Please connect your board to your PC with a USB cable and see if a mass storage\ndevice is detected. Try to open the image inside the device.\n Do you see something interesting ? ;-) (y/N)"
		test_result=$response
		ask_user "Please unmount mass storage device of your PC, unplug USB cable and then press ENTER"
		rmmod g_file_storage
		rm /tmp/backing_file
		if [ "$test_result" == "y" ] || [ "$test_result" == "yes" ]; then
                        echo_test_ok
		else
			exit_failed
                fi
	else
		echo "UDC core not launched"
	fi
}

test_usb_gadget

