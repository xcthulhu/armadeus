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


test_setup()
{
	if [ "$1" == "APF9328" ]; then
		ask_user "Please connect EOC (ADC pin 11 & CSI pin 12) and CVNST (ADC pin 10 & CSI pin 9), then press ENTER"
		modprobe max1027
		ADC_SPI_SYS="/sys/bus/spi/devices/spi1.0"
	elif [ "$1" == "APF27" ]; then
		modprobe max1027
		ADC_SPI_SYS="/sys/bus/spi/devices/spi0.0"
	elif [ "$1" == "APF51" ]; then
		modprobe hwmon
		modprobe as1531
	else
		echo "Platform not supported by this test"
	fi
	return $?
}

test_running_max1027()
{
	# Slow mode
	let set=0x62; echo $set > $ADC_SPI_SYS/setup
	let conv=0xf9; echo $conv > $ADC_SPI_SYS/conversion
	echo "WAIT" > /dev/null
	temp=`cat $ADC_SPI_SYS/temp1_input`
	AIN0=`cat $ADC_SPI_SYS/in0_input`
	if [ "$temp" != "0" ] && [ "$AIN0" != "0" ]; then
		echo "Temp: $temp m°C"
		echo "AIN0: $AIN0"
	else
		exit_failed
	fi
	# Fast mode
	loadmax.sh
	let set=0xb1; echo $set > $ADC_SPI_SYS/conversion
	sleep 1
	let set=0x48; echo $set > $ADC_SPI_SYS/setup
	let set=0x20; echo $set > $ADC_SPI_SYS/averaging
	echo "Reading in fast mode:"
	dd if=/dev/max1027/AIN0 bs=2 count=10 | hexdump
}

test_running_as1531()
{
	ask_user "Connect (jumper) pin 11 (AIN1) and 13 (GND) of ADC connector, then press ENTER"
	VALUE=`cat /sys/bus/spi/devices/spi0.0/in1_input`
	if [ "$VALUE" -gt 3 ]; then
		exit_failed
	fi
	ask_user "Connect (jumper) pin 5 (REF) and 6 (AIN6)  of ADC connector, then press ENTER"
	VALUE=`cat /sys/bus/spi/devices/spi0.0/in6_input`
	if [ "$VALUE" -lt 2497 ]; then
		exit_failed
	fi
	return 0
}

test_running()
{
	if [ "$1" == "APF9328" ]; then
		test_running_max1027
	elif [ "$1" == "APF27" ]; then
		test_running_max1027
	elif [ "$1" == "APF51" ]; then
		test_running_as1531
	else
		echo "Platform not supported by this test"
	fi
	return $?
}

test_ADC()
{
	show_test_banner "ADC"

	execute_for_target test_setup
	if [ "$?" == 0 ]; then
		execute_for_target test_running
		if [ "$?" == 0 ]; then
			echo_test_ok	
		else
			exit_failed
		fi
	else
		echo "Hardware not found !"
	fi
}

test_ADC

