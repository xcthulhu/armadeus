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


echo_jumper_help_apf9328()
{
	echo " (Pin 1 & 3 of UART connector)"
}

echo_jumper_help_apf27()
{
        echo " (Pin 2 & 3 of J21 connector (between RS and J8))"
}

test_serial_port()
{
	show_test_banner "Serial port $1"
	SERIAL_DEVICE="/dev/ttySMX$1"
	TEMP_FILE="/tmp/serial_test$1"
	echo "$SERIAL_DEVICE"
	
	stty -F $SERIAL_DEVICE -echo
	if [ "$?" == 0 ]; then
		echo -n "Connect loopback jumper"
		execute_for_target echo_jumper_help_apf9328 echo_jumper_help_apf27
		ask_user "and then press <ENTER>"
		cat $SERIAL_DEVICE > $TEMP_FILE & 
		pid=$!
		echo -n "."; sleep 1
		echo "Serial Port N°$1 is working !" > $SERIAL_DEVICE
		echo -n "."; sleep 1; echo
		kill $pid
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

echo_apf9328_ports()
{
	echo "    Available one: [ 1 ]"
}

echo_apf27_ports()
{
        echo "    Available one: [ 2 ]"
}

if [ "$1" == "" ]; then
	echo "Please give the port number to test !"
	execute_for_target echo_apf9328_ports echo_apf27_ports
	exit 1
fi

test_serial_port $1

