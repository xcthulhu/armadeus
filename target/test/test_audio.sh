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

TEMP_FILE=/tmp/toto.wav

end_of_test_for_apf9328()
{
	echo_test_ok
	exit 0
}

continue_for_apf27()
{
	true
}

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

	ask_user "Please connect a earphone to the Audio Out connector (top on APF27). Then press ENTER."
	aplay /usr/share/sounds/alsa/Side_Left.wav	
	if [ "$?" == 0 ]; then
		ask_user "Did you hear something ? (y/N)"
		if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
			exit_failed
		fi
	fi

	execute_for_target end_of_test_for_apf9328 continue_for_apf27

	ask_user "Please connect a microphone to the Audio In connector (bottom on APF27). Then press ENTER."
	ask_user "You will now have to speak in the MIC. The sampled sound will then be played back. Press ENTER when ready to speak."
	# Stereo 16bits @ 16KHz
	arecord -r 16000 -f S16_LE -c 2 $TEMP_FILE & pid=$!
	ask_user "You can now speak. Press ENTER when finished."
	kill $pid
	sleep 1
	aplay $TEMP_FILE
	if [ "$?" != 0 ]; then
		exit_failed
	fi
	ask_user "Did you hear what you said before ? (y/N)"
	if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
		exit_failed
	fi

	echo_test_ok
	exit 0
}

test_sound

