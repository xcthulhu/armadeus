#!/bin/ash

#
# This script will load Armadeus GPIO modules
#

GPIO_DEV_DIR=/dev/gpio

if [ -d "$1" ]; then
    GPIO_MODULES_DIR="$1"
else
    KERNEL_VERSION=`uname -r`
    GPIO_MODULES_DIR="/lib/modules/$KERNEL_VERSION/extra/gpio/"
    echo "Using $GPIO_MODULES_DIR as default modules dir as no param was passed"
fi

# Load GPIO main module
#insmod $GPIO_MODULES_DIR/gpio.ko portB_init=0,0,0x00FFFF00,0,0,0,0,0,0xF00FFFFF,0,0,0,0,0,0,0x0FF00000 portD_init=0,0xFFFF0000,0xFFFFFFFF,0,0,0,0,0,0xFFFFFFFF,0,0,0,0,0,0,0xFFFFFC00
insmod $GPIO_MODULES_DIR/gpio.ko
# Load PPDEV emulation module
insmod $GPIO_MODULES_DIR/ppdev.ko port_mode=4

GPIO_MAJOR=`cat /proc/devices | grep GPIO | cut -d " " -f 1`
PPDEV_MAJOR=`cat /proc/devices | grep ppdev | cut -d " " -f 1`

if [ "$GPIO_MAJOR" == "" ]; then
	echo "GPIO module not loaded, exiting"
	exit 1
fi

mkdir -p /dev/parports
mkdir -p $GPIO_DEV_DIR

# For PPDEV
if [ "$PPDEV_MAJOR" != "" ]; then
	mknod /dev/parports/0 c $PPDEV_MAJOR 0
fi

# /dev for "full" port access:
mknod $GPIO_DEV_DIR/portA c $GPIO_MAJOR 255
mknod $GPIO_DEV_DIR/portB c $GPIO_MAJOR 254
mknod $GPIO_DEV_DIR/portC c $GPIO_MAJOR 253
mknod $GPIO_DEV_DIR/portD c $GPIO_MAJOR 252

# /dev for individual pin access:
# PortA[0-31]
for pin in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31; do
	minor=$pin
	mknod $GPIO_DEV_DIR/PA$pin c $GPIO_MAJOR $minor
done
# PortB[8-31]
for pin in 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31; do
	minor=$((32+$pin))
	mknod $GPIO_DEV_DIR/PB$pin c $GPIO_MAJOR $minor
done
# PortC[3-17]
for pin in 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17; do
	minor=$((64+$pin))
	mknod $GPIO_DEV_DIR/PC$pin c $GPIO_MAJOR $minor
done
# PortD[6-31]
for pin in 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31; do
	minor=$((96+$pin))
	mknod $GPIO_DEV_DIR/PD$pin c $GPIO_MAJOR $minor
done


