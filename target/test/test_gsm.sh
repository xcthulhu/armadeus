#!/bin/sh

#
# Script to test Armadeus Software release
#
#  Copyright (C) 2010 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

source ./test_helpers.sh

DEFAULT_PIN_CODE="0000"
DEFAULT_PHONE_NUMBER="0954000000"
GSM_DEVICE=/dev/ttySMX1

send_at_cmd()
{
#	echo "($1)"
	echo -e -n "$1\015" > $GSM_DEVICE
	sleep 1
}

exit_failed_gsm()
{
	killall cat 2>/dev/null
	send_at_cmd ATE1	# ECHO ON
	exit_failed
}

test_gsm()
{
	show_test_banner "GSM (APW)"

	ask_user "Do you have APF27 with an APW wireless extension board with GSM modem and the corresponding Linux kernel ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		echo "Only the APW is supported by this test yet !!"
		exit_failed
	fi

	stty -F $GSM_DEVICE raw -echo -echoe -echok
	if [ "$?" != 0 ]; then
		echo "Failed to setup serial port ($GSM_DEVICE)"
		exit_failed
	fi

	send_at_cmd ATE0	# ECHO OFF
	cat $GSM_DEVICE &
	echo " --- Checking MoDem is responding:"
	send_at_cmd ATI3
	echo " --- Constructor:"
	send_at_cmd AT+CGMI
	echo " --- Model:"
	send_at_cmd AT+CGMM

	ask_user "Please enter your PIN code (ENTER to skip)"
	if [ "$response" == "" ]; then
		PIN_CODE=$DEFAULT_PIN_CODE
	else
		PIN_CODE=$response
		DIGITS=`echo -n "$PIN_CODE" | wc -c`
		if [ "$DIGITS" == 4 ]; then
			echo -e -n "AT+CPIN=\"$PIN_CODE\"\015" > $GSM_DEVICE
			sleep 2
			send_at_cmd "AT+CPIN?"
		else
			echo "Bad PIN code ($PIN_CODE)"
			exit_failed_gsm
		fi
	fi

	ask_user "Please enter a phone number, I will make it ring (default=$DEFAULT_PHONE_NUMBER)"
	if [ "$response" == "" ]; then
		PHONE_NUMBER=$DEFAULT_PHONE_NUMBER
	else
		PHONE_NUMBER=$response
	fi
	DIGITS=`echo -n "$PHONE_NUMBER" | wc -c`
	if [ "$DIGITS" != 10 ]; then
		echo "Bad phone number format ($PHONE_NUMBER)"
		exit_failed_gsm
	fi
	killall cat
	/usr/sbin/chat ABORT 'NO CARRIER' ECHO ON '' AT OK ATD"$PHONE_NUMBER" OK > $GSM_DEVICE < $GSM_DEVICE
	sleep 5
	send_at_cmd ATH
	ask_user "Did you ear it ring ? (Y/n)"
	if [ "$response" == "n" ] || [ "$response" == "no" ]; then
		echo "Probably no carrier ?"
		exit_failed_gsm
	fi
	
	echo_test_ok
	exit 0
}

test_gsm

