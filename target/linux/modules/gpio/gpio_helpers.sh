#!/bin/sh

#
# Functions to ease GPIO settings/usage
#
#  Copyright (C) 2010 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

GPIO_DEV_DIR=/dev/gpio
GPIO_PROC_DIR=/proc/driver/gpio

setbit()
{
	PORT=$1
	NB=$2
	VAL=$3
	cat $PORT | sed "s/[0-1]\([0-1]\{$NB\}\)$/$VAL\1/" > $PORT
}

gpio_set_value()
{
	GPIO=$1
	VALUE=$2
	if [ -c "$GPIO_DEV_DIR/$GPIO" ]; then
		if [ "$VALUE" == "1" ]; then
			echo -ne "\x01" > $GPIO_DEV_DIR/$GPIO
		else
			echo -ne "\x00" > $GPIO_DEV_DIR/$GPIO
		fi
	else
		echo "$GPIO GPIO not usable !"
	fi
}

# Sets $1 as GPIO; $2 == 1 -> outpout, 0 -> input
gpio_mode()
{
        GPIO=`echo $1 | cut -c 3-4`
        VALUE=$2
        PORT=`echo $1 | cut -c 2`
        PORT_DIR=$GPIO_PROC_DIR"/port"$PORT"dir"
        PORT_MODE=$GPIO_PROC_DIR"/port"$PORT"mode"

        setbit $PORT_MODE $GPIO 1
        setbit $PORT_DIR $GPIO $VALUE
}

