#!/bin/sh

#
#  Helpers for Armadeus Software release's test scripts
#
#  Copyright (C) 2008-2010 The Armadeus Project
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

	echo -e "\n  >>>> $MESSAGE <<<<"
	read response
}

ask_user_banner()
{
        echo
        echo -e "--- "$1
}

answer=""
# $1: thing to ask, $* possibilities. Will fill in "answer" variable.
ask_user_choice()
{
	ask_user_banner "$1"
	shift
	n=1
	while [ "$1" != "" ]; do
		echo -n "  $n) "$1

		if [ "$n" == 1 ]; then
			echo " (default)"
		else
			echo ""
		fi
		eval tempvar$n=\"$1\"
		shift
		let n=n+1
	done
	read -p "> " answer

	# If user didn't enter a correct value, use default (first)
	for i in `seq $n`; do
		if [ $i == "$answer" ]; then
			break
		fi
	done
	if [ $i -lt $n ]; then
		eval answer="\$tempvar$answer"
	else
		eval answer="\$tempvar1"
	fi
}

show_test_banner()
{
	echo -e "\033[1m******** Testing $1 ********\033[0m"
}

echo_test_ok()
{
	echo "Test OK !"
}

echo_test_failed()
{
	echo "Test FAILED !"
}

exit_failed()
{
	echo_test_failed
	exit 1
}

is_package_installed()
{
        EXE=`which $1 | head -n 1`
        if [ -x "$EXE" ]; then
                debug "$EXE"
                return 0
        else
		echo "$1 is not installed !"
                return 1
        fi
}

wake_up_lcd()
{
	# Wake up LCD
	echo 0 > /sys/class/graphics/fb0/blank
	sleep 1
}

execute_for_target()
{
	BOARD=`cat /etc/machine`
	if [ "$BOARD" == "APF9328" ] && [ "$1" != "" ]; then
                $1
        fi
        if [ "$BOARD" == "APF27" ] && [ "$2" != "" ]; then
                $2
        fi
}

# $1: name of the file in $TEMP_DIR
# $2: size of the file
create_random_file()
{
	if [ "$1" == "" ] || [ "$2" == "" ]; then
		echo "create_random_file(): missing parameter"
		return 1
	fi
	RANDOM_FILE="$1"
	FILE_SIZE="$2"
	# Create temp bench file
	echo -e "\nCreating a temporary bench file ($TEMP_FILE) of $TEMP_FILE_SIZE KBytes"
	if [ ! -f "$RANDOM_FILE" ]; then
		dd if=/dev/urandom of=$RANDOM_FILE bs=1024 count=$FILE_SIZE 2>/dev/null & pid=$!
		# Show a kind of progress bar
		echo -en "[                          ]\r["
		while [ $? == 0 ]; do
			echo -n "."
			sleep 1
			kill -USR1 $pid 2>/dev/null
		done
		echo
	fi
	echo " -> done"
}

