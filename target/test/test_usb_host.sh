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


load_usb_host_driver_apf9328()
{
        modprobe apf9328-isp1761
        modprobe hal_imx
        modprobe pehci
        if [ "$?" != 0 ]; then
                echo "Unable to load USB Host modules !!"
        fi
}

test_usb_host()
{
	show_test_banner "USB Host"

	execute_for_target load_usb_host_driver_apf9328 echo
	dmesg | grep "USB hub found"
	if [ "$?" != 0 ]; then
		echo "USB Host was not found !!"
		exit_failed
	fi

	echo "Please insert a USB key (nothing will be erased)"
	it=0
	false
	while [ "$?" != 0 ] && [ $it -le 15 ]; do
		it=$((it+1))
		sleep 2
		dmesg | tail | grep "Attached SCSI removable disk"
	done

	mount `ls /dev/sd*1` /mnt/mmc && ls /mnt/mmc/
	if [ "$?" == 0 ]; then
		df -h
		echo_test_ok
	fi
	umount /mnt/mmc
}

test_usb_host

