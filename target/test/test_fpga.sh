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


load_led_apf9328()
{
	dd if=./data/fpga/blinking_led_apf9328_200k.bit of=$FPGA_LOAD_DEV
}

load_led_apf27()
{
	echo "I hope you have connected Pins 1 & 39 of J20 (power)"
	dd if=./data/fpga/blinking_led_apf27_200k.bit of=$FPGA_LOAD_DEV
}

load_button_apf9328()
{
        dd if=./data/fpga/wishbone_example_apf9328_200k.bit of=$FPGA_LOAD_DEV
}

load_button_apf27()
{
        echo "I hope you have connected Pins 1 & 39 of J20 (power)"
        dd if=./data/fpga/wishbone_example_apf27_200k.bit of=$FPGA_LOAD_DEV
}


test_fpga_load()
{
	show_test_banner "FPGA loading"

	modprobe fpgaloader
	RES=$?
	sleep 1
	if [ "$RES" != 0 ] || [ ! -c "$FPGA_LOAD_DEV" ] ; then
		echo "Module failed to load"
		exit_failed
	fi

	cat /proc/driver/fpga/loader

	execute_for_target load_led_apf9328 load_led_apf27
	
	if [ "$?" == 0 ]; then
		ask_user "Did you see the FPGA's LED blinking ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
	rm -f $TEST_BITFILE
}

test_fpga_it()
{
	show_test_banner "FPGA interrupts"

	execute_for_target load_button_apf9328 load_button_apf27
	modprobe irq_ocore
	modprobe gbutton
	modprobe board_buttons

        if [ "$?" != 0 ] ; then
                echo "Some Modules failed to load"
                exit_failed
        fi
	DEVICE_NODE=`cat /proc/devices | grep BUTTON | cut -d " " -f 1`
	mknod /dev/button0 c $DEVICE_NODE 0

	/usr/bin/testsuite/testbutton /dev/button0 &
	PID=$!

	if [ "$?" == 0 ]; then
		ask_user "Please press FPGA button. Did you see something on the console ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
	kill $PID
	rm -f $TEST_BITFILE
}

test_fpga_load
test_fpga_it

