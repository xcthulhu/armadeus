#
#	THE ARMADEUS PROJECT
#
#  Copyright (C) year  The armadeus systems team [Jérémie Scheer]
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

modprobe -r smsc95xx
modprobe -r g_ether

modprobe g_ether

#   shutdown other ports
ifconfig usb0   down 2>/dev/null
ifconfig eth0   down 2>/dev/null
ifconfig eth1   down 2>/dev/null

ifconfig usb0  192.168.10.1

echo -e "\nTo check the USB Gadget port (usb0), please browse on 192.168.10.1\n"

