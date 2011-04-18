#!/bin/sh

modprobe can
modprobe can-dev
modprobe can-raw
modprobe mcp251x
ip link set can0 up type can bitrate 125000
ifconfig can0 up

exit 0
