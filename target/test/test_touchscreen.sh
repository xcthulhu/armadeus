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


test_touchscreen()
{
	show_test_banner "Touchscreen"

	modprobe spi_imx
	modprobe tsc2102_ts
	sleep 1

	dmesg | tail | grep "TSC2102 touchscreen driver initialized"
	if [ "$?" != 0 ]; then
		echo "Hardware not found !"
		exit_failed
	fi

	if [ ! -e "/etc/ts.conf" ]; then
		echo "Tslib misconfigured"
		exit_failed
	fi

	ts_finddev /dev/input/event0 10
	if [ "$?" != 1 ]; then
		echo "Can not communicate with Touchscreen"
		exit_failed
	fi

	echo_test_ok
}

test_touchscreen
