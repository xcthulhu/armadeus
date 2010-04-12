#!/bin/ash

#
# This script will load Armadeus GPIO modules
#

GPIO_DEV_DIR=/dev/gpio

# Load GPIO main module
#modprobe gpio portB_init=0,0,0x00FFFF00,0,0,0,0,0,0xF00FFFFF,0,0,0,0,0,0,0x0FF00000 portD_init=0,0xFFFF0000,0xFFFFFFFF,0,0,0,0,0,0xFFFFFFFF,0,0,0,0,0,0,0xFFFFFC00
modprobe gpio
# Load PPDEV emulation module
if [ "$1" != "" ]; then
	modprobe ppdev port_mode=4
fi

GPIO_MAJOR=`cat /proc/devices | grep GPIO | cut -d " " -f 1`
PPDEV_MAJOR=`cat /proc/devices | grep ppdev | cut -d " " -f 1`

if [ "$GPIO_MAJOR" == "" ]; then
	echo "GPIO module failed to load, exiting..."
	exit 1
fi

if [ -d "$GPIO_DEV_DIR" ]; then
	echo $GPIO_DEV_DIR"xxx devices already created, skipping..."
	exit 0
fi

mkdir -p $GPIO_DEV_DIR

# For PPDEV
if [ "$PPDEV_MAJOR" != "" ]; then
	mkdir -p /dev/parports
	mknod /dev/parports/0 c $PPDEV_MAJOR 0
fi

BOARD=`cat /etc/machine`

# /dev for "full" port access:
mknod $GPIO_DEV_DIR/portA c $GPIO_MAJOR 255
mknod $GPIO_DEV_DIR/portB c $GPIO_MAJOR 254
mknod $GPIO_DEV_DIR/portC c $GPIO_MAJOR 253
mknod $GPIO_DEV_DIR/portD c $GPIO_MAJOR 252
if [ "$BOARD" == "APF27" ]; then
mknod $GPIO_DEV_DIR/portE c $GPIO_MAJOR 251
mknod $GPIO_DEV_DIR/portF c $GPIO_MAJOR 250
fi

# params: $1=port, $2=pin start, $3=pin end, $4=minor start
create_dev()
{
	for pin in `seq $2 $3`; do
		minor=$(($4+$pin))
		mknod $GPIO_DEV_DIR/P$1$pin c $GPIO_MAJOR $minor
	done
	echo -n "."
}

echo -n "Creating /dev nodes for individual pin access: "
if [ "$BOARD" == "APF9328" ]; then
	# PortA[0-31]
	create_dev A 0 31 0
	# PortB[8-31]
	create_dev B 8 31 32
	# PortC[3-17]
	create_dev C 3 17 64
	# PortD[6-31]
	create_dev D 6 31 96
	echo
fi

if [ "$BOARD" == "APF27" ]; then
	# PortA[0-31]
	create_dev A 0 31 0
	# PortB[0-31]
	create_dev B 0 31 32
	# PortC[5-31]
	create_dev C 5 31 64
	# PortD[16-31]
	create_dev D 16 31 96
	# PortE[3-23]
	create_dev E 3 23 128
	# PortF[7-22]
	create_dev F 7 22 160
	echo
fi
