#!/bin/sh

#
# Script to test a Buildroot package for Armadeus Software release
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

#DEBUG=True
EXEC_NAME="mplayer"
VIDEO_FILE_NAME="$TEMP_DIR/apf27.mpg"

test_mplayer()
{
	show_test_banner "Mplayer"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		wake_up_lcd
		# Stop blinking cursor
		echo 0 > /sys/class/graphics/fbcon/cursor_blink
		# Get video
		echo "Downloading file from Internet"
		# Suppose that network was correctly set before
		wget http://dl.free.fr/oBjrVd6n8 -O $VIDEO_FILE_NAME
		# Launch it
		$EXEC_NAME -nosound -fs $VIDEO_FILE_NAME
		if [ "$?" == 0 ]; then
			ask_user "Was video correctly displayed ? If OK say y"
			if [ "$response" == "y" ]; then
				echo_test_ok
				exit 0
			fi
		fi
	fi
	# Make cursor blink again
	echo 1 > /sys/class/graphics/fbcon/cursor_blink
	rm -f "$VIDEO_FILE_NAME"
	exit_failed
}

test_mplayer

