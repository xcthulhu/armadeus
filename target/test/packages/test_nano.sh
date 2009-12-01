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

# Validates: nano and ncurses
EXEC_NAME="nano"
TEST_FILE=/tmp/nano.txt

test_nano()
{
	show_test_banner "nano (Text Editor)"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	ask_user "After having press ENTER, nano will be launched.\n Write a small text in it, then press Ctrl+O to save and Ctrl+X to quit"

	# Launch it
	$EXEC_NAME $TEST_FILE

	if [ "$?" == 0 ]; then
		clear
		cat $TEST_FILE
		echo
		ask_user "Is it the text you typed ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
	fi

	exit_failed
}

test_nano

