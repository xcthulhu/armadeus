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

THIS_DIR=`dirname $0`
source $THIS_DIR/test_env.sh
source $THIS_DIR/test_helpers.sh

mkdir -p $TEMP_DIR
date

show_test_banner "All supported drivers/packages"
echo "Your test environments variables are:"
echo "    - temporary directory: $TEMP_DIR"
echo "    - Host IP: $SERVER_IP"
ask_user "Please confirm that these variables are correct (y/N)"
if [ "$response" != "y" ] && [ "$response" != "yes" ]; then
	echo "Please update $THIS_DIR/test_env.sh"
	exit 1
fi

# Here we go ! (First start with Linux)
$THIS_DIR/test_ethernet.sh
echo
$THIS_DIR/test_usb_host.sh
echo
$THIS_DIR/test_rs232.sh
echo
$THIS_DIR/test_usb_device.sh
echo
$THIS_DIR/test_backlight.sh
echo
$THIS_DIR/test_framebuffer.sh
echo
$THIS_DIR/test_touchscreen.sh
echo
$THIS_DIR/test_dac.sh
echo
$THIS_DIR/test_adc.sh
echo
$THIS_DIR/test_audio.sh
echo
$THIS_DIR/test_fpga.sh
echo
$THIS_DIR/test_gpio.sh
echo
$THIS_DIR/test_pwm.sh
echo
$THIS_DIR/media_perf.sh speed
# Following tests needs Internet access:
ask_user "I will now try to configure Internet access. Press ENTER when ready"
$THIS_DIR/dhcp.sh
echo
$THIS_DIR/test_rtc.sh

# Then check packages:
PACKAGES_TESTS=`ls $THIS_DIR/packages/*.sh`
for package_test in $PACKAGES_TESTS; do
	if [ -x $package_test ] && [ ! -L $package_test ]; then
		$package_test
	fi
done

exit 0

