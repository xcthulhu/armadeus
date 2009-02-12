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

TOLERANCE=1
TEST_DURATION=10

convert_in_secs()
{
	TIME=`echo $1 | sed "s/:0/:/g"`
	
	HOUR=`echo $1 | cut -d ":" -f 2 | sed 's/0*//'`   # sed is to remove leading 0
	MIN=`echo $1 | cut -d ":" -f 3 | sed 's/0*//'`
	SEC=`echo $1 | cut -d ":" -f 4 | sed 's/0*//'`
	
	TIME_SEC=$(($HOUR*3600 + $MIN*60 + $SEC))
	echo "$TIME_SEC"
}

test_rtc()
{
	show_test_banner "RTC"

	ntpdate 217.147.208.1
	date
	if [ "$?" == 0 ]; then
		echo "Please wait "$TEST_DURATION"s"
		hwclock -wu
		DATE=`date +:%T`
		BEFORE=`convert_in_secs $DATE`
		sleep $TEST_DURATION
		hwclock -su
		DATE=`date +:%T`
		AFTER=`convert_in_secs $DATE`
		DIFF=$(($AFTER - $BEFORE - $TEST_DURATION))

		if [ "$DIFF" -gt "$TOLERANCE" ]; then
			echo "$AFTER $BEFORE ($DIFF)"
			exit_failed
		else
			echo_test_ok
		fi
	fi
}

test_rtc

