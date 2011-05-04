#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2008-2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
source ./test_env.sh

TEMP_FILE=/tmp/toto.wav

test_init()
{
	if [ "$1" == "APF9328" ]; then
		modprobe snd-imx-alsa-tsc2102
	elif [ "$1" == "APF27" ]; then
		modprobe snd-imx-alsa-tsc2102
	elif [ "$1" == "APF51" ]; then
		modprobe snd-soc-imx-fiq
		modprobe snd-soc-wm8960
		modprobe snd-soc-apf51dev-wm8960
		amixer cset numid=40 on
		amixer cset numid=37 on
	else
		echo "Platform not supported by this test"
	fi
}

end_of_test_for_apf9328_and_continue_for_others()
{
	if [ "$1" == "APF9328" ]; then
		echo_test_ok
		exit 0
	elif [ "$1" == "APF27" ]; then
		true
	elif [ "$1" == "APF51" ]; then
		true
	else
		echo "Platform not supported by this test"
	fi
}

test_audio_in_init()
{
	if [ "$1" == "APF27" ]; then
		amixer cset numid=7 on
		amixer cset numid=6 30
	elif [ "$1" == "APF51" ]; then
		echo
	else
		echo "Platform not supported by this test"
	fi
}

test_audio()
{
	show_test_banner "Sound / ALSA"

	execute_for_target test_init
	if [ "$?" != 0 ]; then
		exit_failed
	fi

	cat /proc/asound/version
	cat /proc/asound/cards
	aplay -lL
	if [ "$?" != 0 ]; then
		exit_failed
	fi

	ask_user "Please connect an earphone to the Audio Out connector (top one on APF27). Then press ENTER."
	aplay /usr/share/sounds/alsa/Side_Left.wav	
	if [ "$?" == 0 ]; then
		ask_user "Did you hear something ? (y/N)"
		if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
			exit_failed
		fi
	fi

	execute_for_target end_of_test_for_apf9328_and_continue_for_others

	ask_user "Please connect a microphone to the Audio In connector (bottom one on APF27). Then press ENTER."
	execute_for_target test_audio_in_init
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

test_audio

