#
#	THE ARMADEUS PROJECT
#
#  Copyright (C) 2010  The armadeus systems team [Jérémie Scheer]
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
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 

modprobe -r g_ether
modprobe -r g_serial

modprobe g_serial

sleep 4

if [ ! -c /dev/ttyGS0 ]; then
	mknod /dev/ttyGS0 c 249 0
fi

echo -e "\nTo test the RS232 Gadget device, you first have to load usbserial module on your host PC:
sudo modprobe usbserial vendor=0x0525 product=0xA4A6
Then you should be able to enter datas through /dev/ttyACM0 on your host PC:
echo \"COUCOU\" > /dev/ttyACM0\n
Listening for host:"

cat /dev/ttyGS0
