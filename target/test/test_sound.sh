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


test_sound()
{
	show_test_banner "Sound / ALSA"

	modprobe snd-imx-alsa-tsc2102
	if [ "$?" != 0 ]; then
		exit_failed
	fi

	cat /proc/asound/version
	cat /proc/asound/cards
	aplay -lL
	if [ "$?" != 0 ]; then
		exit_failed
	fi

	aplay /usr/share/sounds/alsa/Side_Left.wav	
	if [ "$?" == 0 ]; then
		ask_user "Did you hear something ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
}

test_sound
