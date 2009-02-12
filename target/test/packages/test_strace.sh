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
EXEC_NAME="strace"


test_strace()
{
	show_test_banner "strace"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		# Launch it
		$EXEC_NAME echo "HELLO !!"
		if [ "$?" == 0 ]; then
			echo_test_ok
			exit 0
		fi
	fi
	exit_failed
}

test_strace

