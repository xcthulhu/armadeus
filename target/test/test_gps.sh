#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2010 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh

GPS_DEVICE=/dev/ttySMX4

test_gps()
{
	show_test_banner "GPS (APW)"

	ask_user "Do you have an APF27 with an APW wireless extension board and the corresponding Linux kernel ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		echo "Only the APW is supported by this test yet !!"
		exit_failed
	fi

	stty -F /dev/ttySMX4 raw -echo -echoe -echok 38400
	if [ "$?" != 0 ]; then
		echo "Failed to setup serial port ($GPS_DEVICE) !!"
		exit_failed
	fi

	cat $GPS_DEVICE &
	sleep 3
	killall cat

	ask_user "Did you see some NMEA frames ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		exit_failed
	fi

	echo_test_ok
	exit 0
}

test_gps

