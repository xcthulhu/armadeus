#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2008-2009 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
source ./test_env.sh

GPIO_DEV_DIR=/dev/gpio
GPIO_PROC_DIR=/proc/driver/gpio

blink_led_apf27()
{
	cat $GPIO_PROC_DIR/portFdir | sed "s/[0-1]\([0-1]\{14\}\)$/1\1/" > $GPIO_PROC_DIR/portFmode
	cat $GPIO_PROC_DIR/portFdir | sed "s/[0-1]\([0-1]\{14\}\)$/1\1/" > $GPIO_PROC_DIR/portFdir
	for i in `seq 0 5`; do
		echo -ne "\x01" > /dev/gpio/PF14
		usleep 500000
		echo -ne "\x00" > /dev/gpio/PF14
		usleep 500000
	done
}

blink_led_apf9328()
{
	cat $GPIO_PROC_DIR/portDdir | sed "s/[0-1]\([0-1]\{31\}\)$/1\1/" > $GPIO_PROC_DIR/portDmode
	cat $GPIO_PROC_DIR/portDdir | sed "s/[0-1]\([0-1]\{31\}\)$/1\1/" > $GPIO_PROC_DIR/portDdir
	for i in `seq 0 5`; do
		echo -ne "\x00" > $GPIO_DEV_DIR/PD31
		usleep 500000
		echo -ne "\x01" > $GPIO_DEV_DIR/PD31
		usleep 500000
	done
}

test_led_gpio()
{
	show_test_banner "GPIO (LED)"

	loadgpio.sh
	if [ "$?" != 0 ] || [ ! -c "$GPIO_DEV_DIR/portA" ] ; then
		echo "Module failed to load"
		exit_failed
	fi

	execute_for_target blink_led_apf9328 blink_led_apf27

	ask_user "Did you see a LED blinking ? (y/n)"
	if [ "$response" != "y" ]; then
		exit_failed
	fi
	echo_test_ok
	exit 0
}

test_led_gpio
