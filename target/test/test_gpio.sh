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


setbit()
{
	PORT=$1
	NB=$2
	VAL=$3
	cat $PORT | sed "s/[0-1]\([0-1]\{$NB\}\)$/$VAL\1/" > $PORT
}

gpio_set_value()
{
	GPIO=$1
	VALUE=$2
	if [ "$VALUE" == "1" ]; then
		echo -ne "\x01" > $GPIO_DEV_DIR/$GPIO
	else
		echo -ne "\x00" > $GPIO_DEV_DIR/$GPIO
	fi
}

# Sets $1 as GPIO; $2 == 1 -> outpout, 0 -> input
gpio_mode()
{
        GPIO=`echo $1 | cut -c 3-4`
        VALUE=$2
        PORT=`echo $1 | cut -c 2`
        PORT_DIR="/proc/driver/gpio/port"$PORT"dir"
        PORT_MODE="/proc/driver/gpio/port"$PORT"mode"

        setbit $PORT_MODE $GPIO 1
        setbit $PORT_DIR $GPIO $VALUE
}

blink_led_apf27()
{
	LED_NAME="D14"
	gpio_mode PF14 1
	for i in `seq 0 5`; do
		gpio_set_value PF14 0
		usleep 500000
		gpio_set_value PF14 1
		usleep 500000
	done
}

blink_led_apf9328()
{
	LED_NAME="a"
	gpio_mode PD31 1
	for i in `seq 0 5`; do
		gpio_set_value PD31 1
		usleep 500000
		gpio_set_value PD31 0
		usleep 500000
	done
}

check_button_apf27()
{
	setbit /proc/driver/gpio/portFmode 13 1
	setbit /proc/driver/gpio/portFdir 13 0
	setbit /proc/driver/gpio/portFirq 13 1
	echo "Press 2 times on S1 button"
	/usr/bin/testsuite/testbutton /dev/gpio/PF13 3
	if [ "$?" != 0 ]; then
		exit_failed
	fi
}

check_button_apf9328()
{
	echo "TBDL"
}


test_led_gpio()
{
	show_test_banner "GPIO (LED)"

	loadgpio.sh
	if [ "$?" != 0 ] || [ ! -c "$GPIO_DEV_DIR/portA" ]; then
		echo "Module failed to load"
		exit_failed
	fi

	ask_user "I will now blink a LED on your board. Press ENTER to continue"
	execute_for_target blink_led_apf9328 blink_led_apf27

	ask_user "Did you see "$LED_NAME" LED blinking ? (y/N)"
	if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
		exit_failed
	fi

	execute_for_target check_button_apf9328 check_button_apf27

	echo_test_ok
	exit 0
}

test_led_gpio

