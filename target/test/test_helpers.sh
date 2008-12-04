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
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

debug()
{
	if [ "$DEBUG" == "True" ]; then
		echo $1
	fi
}

get_time_in_ms_from_file()
{
	if [ -f "$1" ]; then
		TIME=`cat $1 | grep real | cut -d m -f 2 | sed 's/s//'`
		STIME=`echo $TIME | cut -d . -f 1`
		STIME=`expr $STIME \* 1000`
		MSTIME=`echo $TIME | cut -d . -f 2`
		MSTIME=`expr $MSTIME \* 10`
		NAME=`echo $1 | grep write`
		if [ "$NAME" != "" ]; then
			WTIME=`expr $STIME + $MSTIME`
			debug "WTIME: $WTIME"
		else
			RTIME=`expr $STIME + $MSTIME`
			debug "RTIME: $RTIME"
		fi
	else
		echo "File doesn't exist"
	fi
}

response=""
ask_user()
{
	MESSAGE="$1"
	AWAITEN="$2"

	echo -e "  >>>> $MESSAGE <<<<"
	read response
}

show_test_banner()
{
	echo -e "\033[1m******** Testing $1 ********\033[0m"
}

echo_test_ok()
{
	echo "Test OK !"
}

exit_failed()
{
	echo "Test FAILED !"
	exit 1
}

