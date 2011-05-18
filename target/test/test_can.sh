#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh

show_connection()
{
	if [ "$1" == "APF27" ]; then
		echo "======== APF 27 ========"
		echo "CAN bus connector:"
		echo "__________"
		echo "| * * * * |"
		echo "--4-3-2-1--"
		echo ""
		echo "Please shortcut J11 jumper and connect the second node on the CAN bus:"
		echo "-4 on GND"
		echo "-3 on CANL"
		echo "-2 on CANH"
		ask_user "Then press ENTER"
	elif [ "$1" == "APF51" ]; then
		echo "======== APF 51 ========"
		echo "CAN bus connector:"
		echo "__7_5_3_1_"
		echo "| * * * * |"
		echo "| * * * * |"
		echo "--8-6-4-2--"
		echo ""
		echo "Please shortcut 7 with 8 and connect the second node on the CAN bus:"
		echo "-6 on GND"
		echo "-3 on CANL"
		echo "-4 on CANH"
		ask_user "Then press ENTER"
	else
		echo "Platform not supported by this test"
		exit_failed
	fi
}

test_can()
{
	show_test_banner "CAN bus"

	execute_for_target show_connection
	message_sent=0
	message_received=0

	modprobe can && modprobe can-dev && modprobe can-raw && modprobe mcp251x
        if [ "$?" != 0 ]; then
                echo "Some modules failed to load"
                exit_failed
        fi

	echo $(ip link set can0 up type can bitrate 125000)

	candump can0 -l -s 2 &
	echo $(cansend can0 280#R)
	echo "Please wait..."
	sleep 1

	killall candump

	for line in $(cat candump-*.log); do
		echo "$line"

		if [ "$line" = "280#R" ]; then
			message_sent=1
		fi

		if [ "$line" = "280#" ]; then
			message_received=1
		fi
	done

	rm $(ls candump-*.log)
	rmmod mcp251x
	rmmod can-dev
	rmmod can-raw
	rmmod can

	if [[ $message_sent -eq 1 && $message_received -eq 1 ]]; then
		echo_test_ok
		exit 0
	else
		exit_failed
	fi
}

test_can
