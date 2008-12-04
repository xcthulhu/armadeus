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


test_TV()
{
	show_test_banner "TV ouput"

	imxregs LPCR 0XF8C80085
	imxregs HCR 0x28000500
	imxregs VCR 0x0c001617
	imxregs CSCR 0x03010003
	imxregs SPCTL0 0x04011402
	imxregs SPCTL1 0x00000040

	ch7024 /usr/bin/ch500x320.conf
	if [ "$?" == 0 ]; then
		ask_user "Do you see something on your TV ? (y/n)"
		if [ "$response" == "y" ]; then
			echo_test_ok
		fi
	fi
}

test_TV
