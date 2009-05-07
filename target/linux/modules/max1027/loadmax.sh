#!/bin/ash

#
# This script will load Armadeus Max1027 module and create /dev/xxx entries
#

MAX_DEV_DIR=/dev/max1027

if [ -d "$1" ]; then
	MAX_MODULES_DIR="$1"
	insmod $MAX_MODULES_DIR/max1027.ko
else
	modprobe max1027
fi

MAX_MAJOR=`cat /proc/devices | grep max1027 | cut -d " " -f 1`

if [ "$MAX_MAJOR" == "" ]; then
	echo "Max1027 module not loaded, exiting"
	exit 1
fi

mkdir -p $MAX_DEV_DIR

for channel in 0 1 2 3 4 5 6; do
	minor=$channel
	mknod $MAX_DEV_DIR/AIN$channel c $MAX_MAJOR $minor
done
