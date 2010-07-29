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
EXEC_NAME="boa"
HTML_DIR="/var/www"
HTML_FILE="$HTML_DIR/index.html"
BOA_LOG_DIR="/var/log/boa"

generate_html_test_file()
{
	cat << EOF > $HTML_FILE
<html>
    <head>
        <title> Test of Boa Web Server </title>
    </head>
    <body>
        <h1> If you can read that, then the test is OK ! ;-) </h1>
    </body>
</html>
EOF
}

test_cleanup()
{
	rm -f "$HTML_FILE"
	killall $EXEC_NAME
}

test_boa()
{
	show_test_banner "Boa"

	is_package_installed $EXEC_NAME

	if [ "$?" == 0 ]; then
		mkdir -p $HTML_DIR
		mkdir -p $BOA_LOG_DIR
		grep armadeus /etc/hosts | grep 127.0.0.1
		if [ "$?" != 0 ]; then
			echo "127.0.0.1 `cat /etc/hostname`" >> /etc/hosts
		fi
		generate_html_test_file
		IP=`ifconfig eth0 | grep "inet addr" | cut -d ":" -f 2 | cut -d " " -f 1`
		# Launch it
		$EXEC_NAME
		#
		echo "Now you can access to http://"$IP"/index.html from your web browser"
		sleep 8
		if [ "$?" == 0 ]; then
			ask_user "Did you manage to show the web page ? (y/N)"
			if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
				echo_test_ok
				test_cleanup
				exit 0
			fi
		fi
	fi
	test_cleanup
	exit_failed
}

test_boa

