#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2008-2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
#source ./test_env.sh

do_test()
{
	if [ "$1" == "APF9328" ] || [ "$1" == "APF27" ]; then
		setDAC AB 500
		if [ "$?" == 0 ]; then
			sleep 2
			setDAC AB 0
			echo_test_ok
		else
			echo "Hardware not found !"
		fi
	elif [ "$1" == "APF51" ]; then
		modprobe mcp49x2
		cd /sys/bus/spi/devices/spi0.1/device0
		ls
		echo 500 > out0_raw
		echo 500 > out1_raw
		sleep 2
		echo 0 > out0_raw
		echo 0 > out1_raw
		echo_test_ok
	else
		echo "Platform not supported by this test"
	fi
}

test_DAC()
{
	show_test_banner "DAC"

	ask_user "This test is more probant with vumeters. Connect them and press ENTER."

	execute_for_target do_test
}

test_DAC

