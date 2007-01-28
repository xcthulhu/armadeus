#!/bin/bash

#
# This script will load Armadeus gpio modules 
#

if [ -d "$1" ]; then
    FPGA_MODULES_DIR="$1"
else
	KERNEL_VERSION=`uname -r`
    FPGA_MODULES_DIR="/lib/modules/$KERNEL_VERSION/extra/fpga_devtools"
    echo "Using $FPGA_MODULES_DIR as default modules dir as no param was passed"
fi

# Load FPGA main module
insmod $FPGA_MODULES_DIR/fpgaloader.ko 0

FPGA_MAJOR=`cat /proc/devices | grep fpgaloader | cut -d " " -f 1`
mkdir -p /dev/fpga

# For general usage
mknod /dev/fpga/fpgaloader c $FPGA_MAJOR 0

