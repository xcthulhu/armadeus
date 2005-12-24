#!/bin/bash

#
# This script will load Armadeus gpio modules 
#


insmod /lib/modules/2.6.10-imx1/kernel/gpio.ko
insmod /lib/modules/2.6.10-imx1/kernel/ppdev.ko port_mode=4

GPIO_MAJOR=`cat /proc/devices | grep gpio | cut -d " " -f 1`
PPDEV_MAJOR=`cat /proc/devices | grep ppdev | cut -d " " -f 1`

mkdir -p /dev/parports
mkdir -p /dev/gpio

# For PPDEV
mknod /dev/parports/0 c $PPDEV_MAJOR 0

# For general usage
mknod /dev/gpio/portA c $GPIO_MAJOR 0
mknod /dev/gpio/portB c $GPIO_MAJOR 1
mknod /dev/gpio/portC c $GPIO_MAJOR 2
mknod /dev/gpio/portD c $GPIO_MAJOR 3

#cd /usr/bin/

