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


test_setup_apf9328()
{
	echo "I hope you didn't forget to wire EOC and CVNST !"
	ADC_SPI_SYS="/sys/bus/spi/devices/spi1.0"
}

test_setup_apf27()
{
	ADC_SPI_SYS="/sys/bus/spi/devices/spi0.0"
}

test_ADC()
{
	show_test_banner "ADC"

	modprobe max1027
	if [ "$?" == 0 ]; then
		execute_for_target test_setup_apf9328 test_setup_apf27
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

