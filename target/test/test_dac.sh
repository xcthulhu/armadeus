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


test_DAC()
{
	show_test_banner "DAC"

	setDAC AB 500
	if [ "$?" == 0 ]; then
		setDAC AB 0
		echo_test_ok
	else
		echo "Hardware not found !"
	fi
}

test_DAC
