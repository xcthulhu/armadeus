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

EXEC_NAME="links"
WEB_SITE="www.armadeus.org"

test_links()
{
	show_test_banner "links (textual Web Browser)"

	is_package_installed $EXEC_NAME

	if [ "$?" != 0 ]; then
		exit_failed
	fi

	ask_user "After having press ENTER, the Armadeus Project website will be opened in textual mode.\n Then, to quit press ESC and File->Exit"

	# Launch it
	$EXEC_NAME $WEB_SITE

	if [ "$?" == 0 ]; then
		ask_user "Was website displayed ? (y/N)"
		if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
			echo_test_ok
			exit 0
		fi
	fi

	exit_failed
}

test_links

