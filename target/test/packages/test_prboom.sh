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

# Validates: prboom & SDL_net & SDL_mixer
# Requires: SDL & ALSA OSS emulation
EXEC_NAME="/usr/games/prboom"
EXEC_OPTIONS="-width 320 -height 240 &"

test_prboom()
{
	show_test_banner "PrBoom (SDL port of Doom)"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	# Launch it
	ask_user "After having press ENTER, PrBoom will be launched. Press Ctrl+C at any time to stop."
	modprobe snd-pcm-oss
	modprobe snd-mixer-oss
	$EXEC_NAME $EXEC_OPTIONS

	ask_user "Did you have good time ;-) ? (y/N)"
	if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
		echo_test_ok
		exit 0
	fi

	exit_failed
}

test_prboom

