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


test_usb_host()
{
	show_test_banner "USB Host"

	modprobe apf9328-isp1761
	modprobe hal_imx
	modprobe pehci
	if [ "$?" != 0 ]; then
		echo "Unable to load USB Host modules !!"
	fi
	dmesg | tail | grep "USB hub found"
	if [ "$?" != 0 ]; then
		echo "USB Host was not found !!"
	fi

	echo "Please insert a USB key"
	it=0
	false
	while [ "$?" != 0 ] && [ $it -le 15 ]; do
		it=$((it+1))
		sleep 2
		dmesg | tail | grep "Attached SCSI removable disk"
	done

	mount /dev/sda1 /mnt/mmc
	ls /mnt/mmc/
	if [ "$?" == 0 ]; then
		echo_test_ok
	fi
	umount /mnt/mmc
}

test_usb_host