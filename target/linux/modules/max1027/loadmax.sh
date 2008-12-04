#!/bin/ash

#
# This script will load Armadeus Max1027 module and create /dev/
#

MAX_DEV_DIR=/dev/max1027

if [ -d "$1" ]; then
    MAX_MODULES_DIR="$1"
else
    KERNEL_VERSION=`uname -r`
    MAX_MODULES_DIR="/lib/modules/$KERNEL_VERSION/kernel/drivers/armadeus/max1027/"
    echo "Using $MAX_MODULES_DIR as default modules dir as no param was passed"
fi

insmod $MAX_MODULES_DIR/max1027.ko

MAX_MAJOR=`cat /proc/devices | grep max1027 | cut -d " " -f 1`

if [ "$MAX_MAJOR" == "" ]; then
	echo "Max1027 module not loaded, exiting"
	exit 1
fi

mkdir -p $MAX_DEV_DIR

for channel in 0 1 2 3 4 5 6 7; do
	minor=$channel
	mknod $MAX_DEV_DIR/AIN$channel c $MAX_MAJOR $minor
done
