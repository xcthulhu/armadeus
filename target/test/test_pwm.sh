#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2010-2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

#
# Script to test your PWM: plug either an oscilloscope or a buzzer on the signal
#   and launch this script
#

source ./test_helpers.sh
source ./test_env.sh

SYS_DIR="/sys/class/pwm/pwm0/"
PERIOD="1000 2000 3000 4000 5000 6000 7000 8000 9000 10000"
FREQUENCY="100 200 300 400 500 600 700 800"
DUTY="001 250 333 500 666 750 999"
SLEEP_TIME=4

load_driver()
{
	if [ "$1" == "APF9328" ] || [ "$1" == "APF27" ]; then
		modprobe imx-pwm
		if [ ! -d "$SYS_DIR" ]; then
			echo "can't find /sys/ interface"
			exit_failed
		fi
	elif [ "$1" == "APF51" ]; then
		echo "TBDL"
	else
		echo "Platform not supported by this test"
	fi
}

prepare_test()
{
	if [ "$1" == "APF9328" ]; then
		ask_user "Please connect your oscilloscope probe to pin 4 of X21/Timer connector (GND is on pin 8), then press ENTER when ready"
	elif [ "$1" == "APF27" ]; then
		ask_user "Please connect your oscilloscope probe to pin 4 of J22 connector (GND is on pin 6 or 40), then press ENTER when ready"
	elif [ "$1" == "APF51" ]; then
		ask_user "Please connect your oscilloscope probe to pin 6 of J35 (Extension) connector (GND is on pin 9 or 10), then press ENTER when ready"
	else
		echo "Platform not supported by this test"
	fi
}

test_pwm()
{
	show_test_banner "PWM"

	execute_for_target load_driver

	execute_for_target prepare_test

	ask_user "Press ENTER when ready to check PWM output (frequencies starting at 100Hz)"
	echo 1 > $SYS_DIR/active
	# Test frequency setting
	for freq in $FREQUENCY; do
		echo $freq > $SYS_DIR/frequency
		echo "Setting PWM frequency to $freq Hz"
		sleep $SLEEP_TIME
	done

	ask_user "Press ENTER when ready to check PWM output (periods)"
	# Test period setting
	for period in $PERIOD; do
		echo $period > $SYS_DIR/period
		freq=$((1000000/period))
		echo "Setting PWM period to $period us (-> $freq Hz)"
		sleep $SLEEP_TIME
	done

	ask_user "Press ENTER when ready to check PWM output (duty cycle @ 100Hz)"
	# Test duty cycle setting @ 100Hz -> duty value = positive width signal value
	echo 100 > $SYS_DIR/frequency
	for duty in $DUTY; do
		echo $duty > $SYS_DIR/duty
		echo "Setting duty cycle to $duty/1000 "
		sleep $SLEEP_TIME
	done

	echo 500 > $SYS_DIR/duty
	echo 0 > $SYS_DIR/active
	echo "End of PWM test"

	ask_user "Did you get correct signals ? (y/N)"
	if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
		exit_failed
	fi

	echo_test_ok
	exit 0
}

test_pwm

