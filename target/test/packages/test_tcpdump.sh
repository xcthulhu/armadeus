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

# Validates: tcpdump & libpcap
EXEC_NAME="tcpdump"
EXEC_OPTIONS="-i eth0 -x -v -c 3"

test_tcpdump()
{
	show_test_banner "tcpdump"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	ask_user "After having press ENTER, tcpdump will be launched."

	# Launch it
	$EXEC_NAME $EXEC_OPTIONS

	if [ "$?" == 0 ]; then
		ask_user "Did you see the packets dump ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
	fi

	exit_failed
}

test_tcpdump

