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

#DEBUG=True

test_imxregs()
{
	show_test_banner "imxregs"
	imxregs SIDR
	if [ "$?" == 0 ]; then
		CHIP_ID=`imxregs SIDR | grep SIDR | sed 's/^.*0x/0x/' | cut -d " " -f 1`
		echo "Chip ID: $CHIP_ID"
		if [ "$CHIP_ID" == "0x00d4c01d" ]; then
			echo_test_ok
		fi
	fi
}

test_imxregs
