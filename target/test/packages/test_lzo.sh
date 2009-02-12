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

test_lzo()
{
	show_test_banner "lzo"
	if [ -e "./simple" ]; then
	./simple
		if [ "$?" == 0 ]; then
			echo_test_ok
		else
			echo "returned tuntime error"
			exit_failed
		fi
	else
		echo "file simple does not exist"
		exit_failed
	fi
}

test_lzo
