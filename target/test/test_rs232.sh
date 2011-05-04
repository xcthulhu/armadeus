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
source ./test_env.sh


echo_jumper_help()
{
	if [ "$1" == "APF9328" ]; then
		echo " (Pin 1 & 3 of UART connector)"
	elif [ "$1" == "APF27" ]; then
		echo " (Pin 2 & 3 of J21 connector (between RS and J8))"
	elif [ "$1" == "APF51" ]; then
		echo " (Pin 2 & 3 of RS-232 connector)"
	else
		echo "Platform not supported by this test"
	fi
}

test_serial_port()
{
	show_test_banner "Serial port $1"
	SERIAL_DEVICE="$SERIAL_DEVICES$1"
	TEMP_FILE="/tmp/serial_test$1"
	echo "$SERIAL_DEVICE"
	
	stty -F $SERIAL_DEVICE -echo
	if [ "$?" == 0 ]; then
		echo -n "Connect loopback jumper"
		execute_for_target echo_jumper_help
		ask_user "and then press <ENTER>"
		# Configure port as raw
		stty -F $SERIAL_DEVICE raw
		stty -F $SERIAL_DEVICE -echo -echoe -echok
		# Get incoming data
		cat $SERIAL_DEVICE > $TEMP_FILE & 
		pid=$!
		echo -n "."; sleep 1
		# Send data
		echo "Serial Port N°$1 ($SERIAL_DEVICE) is working !!!!!!!!!!" > $SERIAL_DEVICE
		echo -n "."; sleep 1; echo
		cat /proc/interrupts | grep uart
		kill $pid
		# Check if data were transmitted
		grep "working" $TEMP_FILE
		if [ "$?" == 0 ]; then
			echo_test_ok
		else
			exit_failed
		fi
	else
		echo "  serial port not found !"
	fi
}

echo_available_ports()
{
	if [ "$1" == "APF9328" ]; then
		echo "    Available one: [ 1 ]"
		SERIAL_DEVICES="/dev/ttySMX"
	elif [ "$1" == "APF27" ]; then
	        echo "    Available one: [ 2 ]"
		SERIAL_DEVICES="/dev/ttySMX"
	elif [ "$1" == "APF51" ]; then
		echo "    Available one: [ 0 ]"
		SERIAL_DEVICES="/dev/ttymxc"
	else
		echo "Platform not supported by this test"
	fi
}

show_test_banner "Serial ports"
PORT="$1"
if [ "$1" == "" ]; then
	echo "Please give the port number to test !"
	execute_for_target echo_available_ports
	read -p "> " PORT
	[ "$PORT" == "" ] && exit_failed
fi

test_serial_port $PORT

