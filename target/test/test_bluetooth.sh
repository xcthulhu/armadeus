#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2010-2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
#source ./test_env.sh

attach_bt_chip()
{
	if [ "$1" == "APF27" ]; then
		/usr/sbin/hciattach -s 921600 ttySMX5 csr 921600
	elif [ "$1" == "APF51" ]; then
		/usr/sbin/hciattach -s 921600 ttymxc1 csr 921600
	else
		echo "Platform not supported by this test"
	fi
}

test_bluetooth()
{
	show_test_banner "Bluetooth"

	ask_user "Do you want to test USB or board integrated Bluetooth ? (u/I)"
	if [ "$response" == "u" ]; then
		modprobe btusb
		if [ "$?" != 0 ]; then
			echo "Unable to launch USB<->Bluetooth layer !!"
			exit_failed
		fi
		ask_user "Please insert your USB dongle, then press ENTER"
	else
		execute_for_target attach_bt_chip
	fi

	hciconfig hci0 up piscan
	sleep 1
	hciconfig
	ask_user "Do you see your Bluetooth interface ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		exit_failed
	fi

	ask_user "Be sure that your Host PC has its Bluetooth interface up and running (hciconfig hci0 up piscan), then press ENTER"

	MY_BTADDR=`hcitool dev | grep hci0 | cut -f 3`
	if [ "$MY_BTADDR" == "" ]; then
		echo "unable to get dongle BT MAC"
		exit_failed
	fi

	# Setup serial terminal over BT:
	RFCOMM_CHANNEL=7
	mknod /dev/rfcomm0 c 216 0
	rfcomm -i hci0 listen /dev/rfcomm0 $RFCOMM_CHANNEL &

	ask_user "Launch: \"rfcomm connect 0 $MY_BTADDR $RFCOMM_CHANNEL\" on you PC, then press ENTER"
	getty 38400 /dev/rfcomm0 &
	ask_user "Launch: \"minicom -o\" on you PC, you should see APF login prompt, then press ENTER"
	
	# APW: To be done...

	echo_test_ok
}

test_bluetooth

