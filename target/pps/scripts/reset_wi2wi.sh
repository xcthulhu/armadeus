#
#        This software has been developed by ARMADEUS SYSTEMS
#
# Customer Name: Armadeus
#
# Project Name: PPS
#
# Copyright (C) 2010 : 
# 
# This program is the property of the armadeus systems company or of the 
# final customer as soon as terms & conditions stipulated by the contract
# between the two parties are full-filled .
#
# 	Written by: Jérémie Scheer,,,
#

GPIO_DEV_DIR=/dev/gpio

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
	if [ "$VALUE" == "1" ]; then
		echo -ne "\x01" > $GPIO_DEV_DIR/$GPIO
	else
		echo -ne "\x00" > $GPIO_DEV_DIR/$GPIO
	fi
}

# Sets $1 as GPIO; $2 == 1 -> outpout, 0 -> input
gpio_mode()
{
	GPIO=`echo $1 | cut -c 3-4`
	VALUE=$2
	PORT=`echo $1 | cut -c 2`
	PORT_DIR="/proc/driver/gpio/port"$PORT"dir"
	PORT_MODE="/proc/driver/gpio/port"$PORT"mode"

	setbit $PORT_MODE $GPIO 1
	setbit $PORT_DIR $GPIO $VALUE
}

gpio_mode PE11 1
gpio_set_value PE11 0
gpio_set_value PE11 1
