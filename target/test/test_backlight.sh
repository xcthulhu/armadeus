#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2008-2010 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh

SYS_DIR=/sys/class/backlight/imx-bl

test_backlight()
{
	show_test_banner "Backlight"
	echo 0 > /sys/class/graphics/fb0/blank   # Leave FB sleep mode
	# Paint it white:
	clear > /dev/tty1
	cat /dev/zero | tr '\0' '\377' | dd of=/dev/fb0 bs=1K count=750 2>/dev/null

	modprobe imx_bl
	if [ "$?" != 0 ]; then
		echo "module not found"
		exit 1
	fi
	current=`cat $SYS_DIR/actual_brightness`
	echo "Current brightness level: $current / 256"

	echo 50 > $SYS_DIR/brightness
	ask_user "Backlight set to MIDDLE, press ENTER"
	echo 255 > $SYS_DIR/brightness
	ask_user "Backlight set to MAX, press ENTER"
	echo 0 > $SYS_DIR/brightness
	ask_user "Backlight set to MIN, press ENTER"
	echo $current > $SYS_DIR/brightness

	ask_user "I will now loop through brightness, press ENTER when ready"
	for loop in `seq 1 2 150`; do
		usleep 100000
		echo $loop > $SYS_DIR/brightness
	done
	echo $current > $SYS_DIR/brightness
	ask_user "Did the test succeed ? (y/N)"
	if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
		echo_test_ok
		exit 0
	fi
	exit_failed
}

test_backlight

