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
EXEC_NAME="lua"
LUA_EXAMPLE_FILE="/tmp/test.lua"

generate_test_file()
{
	cat << EOF > $LUA_EXAMPLE_FILE
print "If you can read that then test is OK. Today it's:"
print(os.date())
EOF
}

test_cleanup()
{
	rm -f "$LUA_EXAMPLE_FILE"
#	killall $EXEC_NAME
}

test_lua()
{
	show_test_banner "Lua"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		generate_test_file
		# Launch it
		$EXEC_NAME $LUA_EXAMPLE_FILE
		if [ "$?" == 0 ]; then
			echo_test_ok
			test_cleanup
			exit 0
		fi
	fi
	test_cleanup
	exit_failed
}

test_lua

