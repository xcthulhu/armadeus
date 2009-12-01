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

# Validates: madplay
# Requires: ALSA & Internet access
#DEBUG=True
EXEC_NAME="madplay"
FILE_WEB_ADDRESS="http://www.armadeus.com//assos_downloads/misc/mozart.mp3"
MUSIC_FILE_NAME="$TEMP_DIR/mozart.mp3"

test_madplay()
{
	show_test_banner "MADplay"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	if [ ! -f "$MUSIC_FILE_NAME" ]; then
		# Get music
		echo "Downloading file from Internet"
		# Suppose that network was correctly set before
		wget $FILE_WEB_ADDRESS -O $MUSIC_FILE_NAME
		if [ "$?" != 0 ]; then
			exit_failed
		fi
	fi

	# Launch it
	$EXEC_NAME $MUSIC_FILE_NAME &
	PID=$!
	if [ "$?" == 0 ]; then
		sleep 10
		ask_user "Was music correctly played ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
		kill $PID
	fi

	exit_failed
}

test_madplay

