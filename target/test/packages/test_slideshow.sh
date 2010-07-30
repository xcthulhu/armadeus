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

# Requires: test_sdl.sh
# Validates: libpng & slideshow
EXEC_NAME="slideshow"
TEST_DIR="../data/images/"

test_slideshow()
{
	show_test_banner "slideshow"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	# Launch it
	$EXEC_NAME -width 480 -height 272 -name $TEST_DIR

	if [ "$?" == 0 ]; then
		ask_user "Did you see an image ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
	fi

	exit_failed
}

test_slideshow

