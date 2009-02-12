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
EXEC_NAME="imxregs"

test_imxregs()
{
	show_test_banner "imxregs"
	is_package_installed $EXEC_NAME

	APF=`cat /etc/machine`
	echo -n "Running on an "
	if [ "$APF" == "APF27" ]; then
		ID_REG="CID"
		ID_VAL="0x2882101d"
	else
		ID_REG="SDIR"
		ID_VAL="0x00d4c01d"
	fi
	echo "$APF"
	$EXEC_NAME $ID_REG
	if [ "$?" == 0 ]; then
		CHIP_ID=`$EXEC_NAME $ID_REG | grep $ID_REG | sed 's/^.*0x/0x/' | cut -d " " -f 1`
		echo "Chip ID: $CHIP_ID"
		if [ "$CHIP_ID" == $ID_VAL ]; then
			echo_test_ok
			exit 0
		fi
	fi
	exit_failed
}

test_imxregs

