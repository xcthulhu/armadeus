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


load_led_apf9328()
{
	load_fpga ./data/fpga/blinking_led_apf9328_200k.bit
	RES=$?
}

load_led_apf27()
{
	ask_user "Please connect pins 1 & 39 of J20 connector (3,3V supply for FPGA bank)\nThen press enter"
	load_fpga ./data/fpga/blinking_led_apf27_200k.bit
	RES=$?
}

load_button_apf9328()
{
	load_fpga ./data/fpga/wishbone_example_apf9328_200k.bit
	RES=$?
}

load_button_apf27()
{
	echo "I hope you have connected pins 1 & 39 of J20"
	load_fpga ./data/fpga/wishbone_example_apf27_200k.bit
	RES=$?
}


test_fpga_load()
{
	show_test_banner "FPGA loading"

	execute_for_target load_led_apf9328 load_led_apf27
	
	if [ "$RES" == 0 ]; then
		ask_user "Did you see the FPGA's LED blinking ? (y/N)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		else
			echo_test_failed
		fi
	fi
}

test_fpga_it()
{
	show_test_banner "FPGA interrupts"

	execute_for_target load_button_apf9328 load_button_apf27
	if [ "$RES" != 0 ]; then
		exit_failed
	fi

	modprobe irq_ocore && modprobe gbutton && modprobe board_buttons
	DEVICE_NODE=`cat /proc/devices | grep BUTTON | cut -d " " -f 1`
        if [ "$?" != 0 ] || [ "$DEVICE_NODE" == "" ]; then
                echo "Some modules failed to load"
                exit_failed
        fi
	if [ ! -c /dev/button0 ]; then
		mknod /dev/button0 c $DEVICE_NODE 0
	fi

	/usr/bin/testsuite/testbutton /dev/button0 &
	PID=$!

	if [ "$?" == 0 ]; then
		ask_user "Please press FPGA button. Did you see something on the console ? (y/N)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		else
			echo_test_failed
		fi
	fi
	kill $PID
}

test_fpga_load
test_fpga_it

