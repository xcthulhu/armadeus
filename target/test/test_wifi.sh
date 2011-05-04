#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2010-2011 The Armadeus Project - ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh
source ./test_env.sh

AP_LIST_FILE=/tmp/ap.list
DEFAULT_WEP_KEY="01234567890123456789012345"
FILE_TO_LOAD="test_wifi.bin"
#SERVER_IP set in test_env.sh

get_interface_name()
{
	if [ "$1" == "APF27" ]; then
		WLAN_IF=eth1
	elif [ "$1" == "APF51" ]; then
		WLAN_IF=wlan0
	else
		echo "Platform not supported by this test"
		exit 0
	fi
}

test_wifi()
{
	show_test_banner "WiFi (802.11 b/g)"

	ask_user "This test assumes that you run a private WiFi network (WEP shared key security). Your Access Point (AP) should be connected to your Host. Press ENTER to continue"

	ask_user "Do you have an APW wireless extension board or an APF51Dev ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		echo "Currently, only the APW & the APF51Dev are supported by this test !!"
		exit_failed
	fi

	modprobe libertas_sdio
	if [ "$?" != 0 ]; then
		echo "Failed to load Libertas driver !!"
		exit_failed
	fi
	execute_for_target get_interface_name
	ifconfig $WLAN_IF up
	if [ "$?" != 0 ]; then
		echo "WLAN interface not available !!"
		exit_failed
	fi
	#ifconfig eth0 down

	# get available APs (remove hidden one)
	echo -e "\n Scanning available APs"
	iwlist $WLAN_IF scan | grep ESSID > $AP_LIST_FILE
	AP_LIST=`cat $AP_LIST_FILE | grep -v \"\" | cut -d : -f 2 | sed -e 's/"//g'`

	# ask which one to use
	ask_user_choice "Which one is your AP ?" $AP_LIST
	AP="$answer"
	echo "Choosed: $AP"
	ask_user "Please enter your WEP key (default=$DEFAULT_WEP_KEY)"
	if [ "$response" == "" ]; then
		WEP_KEY=$DEFAULT_WEP_KEY
	else
		WEP_KEY=$response
	fi
	echo "Trying to get associated with $AP (key=$WEP_KEY)"
	iwconfig $WLAN_IF key $WEP_KEY
	iwconfig $WLAN_IF essid $AP
	sleep 3
	associated=`iwconfig $WLAN_IF | grep "Access Point" | grep -c -v "Not-Associated"`
	if [ "$associated" != 1 ]; then
		echo "Failed to get associated with $AP"
		exit_failed
	fi
	echo -e "\n Asking for an IP address"
	udhcpc -i $WLAN_IF
	ask_user "I will now download $FILE_TO_LOAD from your TFTP server ($SERVER_IP).\nBe sure that the file and its correspondig md5sum is available. Then press ENTER."
	echo -e "\n Pinging server:"
	ping -c 3 $SERVER_IP
	if [ "$?" != 0 ]; then
		exit_failed
	fi
	cd /tmp/
	echo -e "\n Downloading $FILE_TO_LOAD:"
	time tftp -g -r $FILE_TO_LOAD $SERVER_IP
	if [ "$?" != 0 ]; then
		exit_failed
	fi

	echo -e "\n Downloading $FILE_TO_LOAD.md5:"
	time tftp -g -r $FILE_TO_LOAD.md5 $SERVER_IP
	md5sum -c $FILE_TO_LOAD.md5
	if [ "$?" != 0 ]; then
		echo "MD5 check KO !!"
		exit_failed
	fi

	echo_test_ok
	exit 0
}

test_wifi

