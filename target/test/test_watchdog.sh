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

WATCHDOG_DEV="/dev/watchdog"
SEC=5

test_watchdog()
{
	show_test_banner "Watchdog"

	ask_user "This test will reboot your board. Continue ? (y/N)"
	if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
		exit_failed
	fi

	modprobe imx-wdt timeout=$SEC
	RES=$?
	sleep 1
	if [ "$RES" != 0 ] || [ ! -c "$WATCHDOG_DEV" ] ; then
		echo "Module failed to load"
		exit_failed
	fi

	watchdog -t $(($SEC-2)) /dev/watchdog
	if [ "$?" != 0 ]; then
		echo "Failed to launch userspace helper"
		exit_failed
	fi

	ask_user "Press ENTER to reboot your system after $SEC seconds"

	killall watchdog
	sleep $(($SEC+2))

	echo "You didn't reboot"
	exit_failed
}

test_watchdog

