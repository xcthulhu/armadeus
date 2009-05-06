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


test_ADC()
{
	show_test_banner "ADC"

	modprobe max1027
	if [ "$?" == 0 ]; then
		# Slow mode
		let set=0x62; echo $set > /sys/bus/spi/devices/spi1.0/setup
		echo "WAIT" > /dev/null
		let conv=0xf9; echo $conv > /sys/bus/spi/devices/spi1.0/conversion
		echo "WAIT" > /dev/null
		temp=`cat /sys/bus/spi/devices/spi1.0/temp1_input`
		if [ "$temp" != "0" ]; then
			echo "Temp: $temp m°C"
			echo_test_ok
		else
			exit_failed
		fi
	else
		echo "Hardware not found !"
	fi
}

test_ADC

