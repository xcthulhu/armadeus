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

test_fpga_load()
{
	show_test_banner "FPGA"

	modprobe fpgaloader
	if [ "$?" != 0 ] || [ ! -c "$FPGA_LOAD_DEV" ] ; then
		echo "Module failed to load"
		exit_failed
	fi

	cat /proc/driver/fpga/loader
	echo "Downloading bitfile from Internet"
	# Suppose that network was correctly set before
	wget http://dl.free.fr/ppbCdFTqs -O $TEST_BITFILE
	
	dd if=$TEST_BITFILE of=$FPGA_LOAD_DEV
	if [ "$?" == 0 ]; then
		ask_user "Did you see the LED on FPGA ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
	rm -f $TEST_BITFILE
}

test_fpga_load

