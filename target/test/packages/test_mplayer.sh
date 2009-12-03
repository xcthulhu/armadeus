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

# Requires: Framebuffer & Internet access
# Validates: mplayer
#DEBUG=True
EXEC_NAME="mplayer"
FILE_WEB_ADDRESS="http://www.armadeus.com//assos_downloads/misc/apf27.mpg"
VIDEO_FILE_NAME="$TEMP_DIR/apf27.mpg"

test_mplayer()
{
	show_test_banner "Mplayer"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	wake_up_lcd
	# Stop blinking cursor
	echo 0 > /sys/class/graphics/fbcon/cursor_blink

	if [ ! -f $VIDEO_FILE_NAME ]; then
		# Get video (suppose that network was correctly set before)
		echo "Downloading file from Internet"
		wget $FILE_WEB_ADDRESS -O $VIDEO_FILE_NAME
	        if [ "$?" != 0 ]; then
	                exit_failed
	        fi
	fi

	# Launch it
	$EXEC_NAME -vo fbdev -nosound -fs $VIDEO_FILE_NAME
	RES=$?
	# Make cursor blink again
	echo 1 > /sys/class/graphics/fbcon/cursor_blink

	if [ "$RES" == 0 ]; then
		ask_user "Was video correctly displayed ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
	fi

	exit_failed
}

test_mplayer

