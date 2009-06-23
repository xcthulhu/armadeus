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

EXEC_NAME="fbtest"

test_framebuffer()
{
	show_test_banner "Framebuffer"
	echo 0 > /sys/class/graphics/fb0/blank   # Leave FB sleep mode
	# Stop blinking cursor
	echo 0 > /sys/class/graphics/fbcon/cursor_blink

	is_package_installed $EXEC_NAME
	$EXEC_NAME
	RES=$?

	# Reactivate cursor
	echo 1 > /sys/class/graphics/fbcon/cursor_blink
	# Ask user for feedbacks
	if [ "$RES" == 0 ]; then
		ask_user "Did all the tests show fine ? If OK say y"
			if [ "$response" == "y" ]; then
				echo_test_ok
				exit 0
			fi
	fi
	exit_failed
}

test_framebuffer
