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
EXEC_NAME="madplay"
MUSIC_FILE_NAME="$TEMP_DIR/mozart.mp3"

test_madplay()
{
	show_test_banner "MADplay"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		# Get music
		echo "Downloading file from Internet"
		# Suppose that network was correctly set before
		wget http://dl.free.fr/hkaOiy3aT -O $MUSIC_FILE_NAME
		# Launch it
		$EXEC_NAME $MUSIC_FILE_NAME
		if [ "$?" == 0 ]; then
			ask_user "Was music correctly played ? If OK say y"
			if [ "$response" == "y" ]; then
				echo_test_ok
				exit 0
			fi
		fi
	fi
	rm -f "$MUSIC_FILE_NAME"
	exit_failed
}

test_madplay

