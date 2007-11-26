#!/bin/bash

#
# This script will load Armadeus gpio modules 
#

if [ -d "$1" ]; then
    GPIO_MODULES_DIR="$1"
else
    KERNEL_VERSION=`uname -r`
    GPIO_MODULES_DIR="/lib/modules/$KERNEL_VERSION/extra/gpio/"
    echo "Using $GPIO_MODULES_DIR as default modules dir as no param was passed"
fi

# Load GPIO main module
insmod $GPIO_MODULES_DIR/gpio.ko portB_init=0,0,0x00FFFF00,0,0,0,0,0,0xF00FFFFF,0,0,0,0,0,0,0x0FF00000 portD_init=0,0xFFFF0000,0xFFFFFFFF,0,0,0,0,0,0xFFFFFFFF,0,0,0,0,0,0,0xFFFFFC00
# Load PPDEV emultaion module
insmod $GPIO_MODULES_DIR/ppdev.ko port_mode=4

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


