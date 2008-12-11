#!/bin/sh

#
# Script to test Armadeus Software release (main launching script)
#
#  Copyright (C) 2008 The Armadeus Project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

source ./test_env.sh

mkdir -p $TEMP_DIR
date

./test_ethernet.sh
echo
./test_usb_host.sh
echo
./test_rs232.sh 1
echo
./test_usb_device.sh
echo
./test_backlight.sh
echo
./test_touchscreen.sh
echo
./test_dac.sh
echo
./test_imxregs.sh
echo
./test_sound.sh
echo

exit 0
