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

FPGA_LOAD_DEV="/dev/fpgaloader"
TEST_BITFILE=/tmp/led.bit


load_apf9328()
{
	dd if=./data/fpga/blinking_led_apf9328_200k.bit of=$FPGA_LOAD_DEV
}

load_apf27()
{
	echo "I hope you have connected Pins 1 & 39 of J20 (power)"
	dd if=./data/fpga/blinking_led_apf27_200k.bit of=$FPGA_LOAD_DEV
}

test_fpga_load()
{
	show_test_banner "FPGA"

	modprobe fpgaloader
	RES=$?
	sleep 1
	if [ "$RES" != 0 ] || [ ! -c "$FPGA_LOAD_DEV" ] ; then
		echo "Module failed to load"
		exit_failed
	fi

	cat /proc/driver/fpga/loader

	execute_for_target load_apf9328 load_apf27
	
	if [ "$?" == 0 ]; then
		ask_user "Did you see the LED on FPGA ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
	rm -f $TEST_BITFILE
}

test_fpga_load

