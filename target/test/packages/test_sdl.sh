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
EXEC_NAME="water"

test_cleanup()
{
	killall $EXEC_NAME
}

test_sdlwater()
{
	show_test_banner "SDL + Touch"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		wake_up_lcd
		# Launch it (needs touchscreen driver to be launched before)
		export SDL_MOUSEDRV=TSLIB
		export SDL_MOUSEDEV=$TSLIB_TSDEVICE
		$EXEC_NAME /usr/share/images/water320.bmp 1>/dev/null &
		echo "Please touch the LCD screen"
		sleep 5
		ask_user "Did you manage to draw circles in water ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			test_cleanup
			exit 0
		fi
		test_cleanup
	fi

	exit_failed
}

test_sdlwater

