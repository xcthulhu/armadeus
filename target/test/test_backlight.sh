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

SYS_DIR=/sys/class/backlight/imx-bl

test_backlight()
{
	show_test_banner "Backlight"
	echo 0 > /sys/class/graphics/fb0/blank   # Leave FB sleep mode

	modprobe imx_bl
	if [ "$?" != 0 ]; then
		echo "module not found"
		exit 1
	fi
	current=`cat $SYS_DIR/actual_brightness`
	echo "Current brightness level: $current / 256"

	echo 50 > $SYS_DIR/brightness
	ask_user "Backlight set to LOW, if OK say y"
	echo 200 > $SYS_DIR/brightness
	ask_user "Backlight set to HIGH, if OK say y"
	if [ "$response" == "y" ]; then
		echo_test_ok
		exit 0
	fi
	exit_failed
}

test_backlight

