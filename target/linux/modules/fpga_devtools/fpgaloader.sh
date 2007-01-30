#!/bin/sh
# Load FPGA main module
modprobe fpgaloader fpga_descriptor=0

FPGA_MAJOR=`cat /proc/devices | grep fpgaloader | cut -d " " -f 1`
mkdir -p /dev/fpga

# For general usage
if [ ! -e /dev/fpga/fpgaloader ] ; then
  mknod /dev/fpga/fpgaloader c $FPGA_MAJOR 0
fi
