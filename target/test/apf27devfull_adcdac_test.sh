#!/bin/sh

#
# Script to test Armadeus APF27Dev ADC/DAC
#
#  Copyright (C) 2008 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

show_test_banner()
{
	echo -e "\033[1m******** Testing $1 ********\033[0m"
}

test_ADCDAC()
{
	show_test_banner "ADC DAC"
	modprobe max1027

	echo "You will need a special hardware to run this test"

	setDAC A 614
	setDAC B 820

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	let set=0x62; echo $set > /sys/bus/spi/devices/spi0.0/setup
	sleep 1
	let conv=0xb0; echo $conv > /sys/bus/spi/devices/spi0.0/conversion
	sleep 1

	result=`cat /sys/bus/spi/devices/spi0.0/in0_input`
        if [ "$result" -gt 1080 ] && [ "$result" -lt 1320 ]; then
            echo "AIN0: ok"
        else
            echo "AIN0: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in1_input`
        if [ "$result" -gt 650 ] && [ "$result" -lt 790 ]; then
            echo "AIN1: ok"
        else
            echo "AIN1: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in2_input`
        if [ "$result" -gt 1300 ] && [ "$result" -lt 1580 ]; then
            echo "AIN2: ok"
        else
            echo "AIN2: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in3_input`
        if [ "$result" -gt 860 ] && [ "$result" -lt 1060 ]; then
            echo "AIN3: ok"
        else
            echo "AIN3: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in4_input`
        if [ "$result" -gt 1080 ] && [ "$result" -lt 1320 ]; then
            echo "AIN4: ok"
        else
            echo "AIN4: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in5_input`
        if [ "$result" -gt 1730 ] && [ "$result" -lt 2110 ]; then
            echo "AIN5: ok"
        else
            echo "AIN5: $result"
        fi

	result=`cat /sys/bus/spi/devices/spi0.0/in6_input`
        if [ "$result" -gt 1080 ] && [ "$result" -lt 1320 ]; then
            echo "AIN6: ok"
        else
            echo "AIN6: $result"
        fi
}

test_ADCDAC

