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

if [ $# -eq 2 ]; then

	#create WPA configuration
	mkdir -p /etc/wpa_supplicant/

echo "ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=wheel
network={
ssid=\"$1\"
scan_ssid=1
proto=WPA
key_mgmt=WPA-PSK
pairwise=TKIP
psk=\"$2\"
}" > /etc/wpa_supplicant/wpa_supplicant.conf

	#Hardware reset of wi2wi chip
	sh /usr/local/pps/scripts/reset_wi2wi.sh

	#Load mmc and libertas modules
	modprobe mxcmmc
	sleep 1
	modprobe libertas_sdio

	#   shutdown other ports
	ifconfig usb0   down 2>/dev/null
	ifconfig eth0   down 2>/dev/null
	ifconfig usb1   down 2>/dev/null

	#Execute Wpa Supplicant
	ifconfig eth1 up
	wpa_supplicant -ieth1 -c /etc/wpa_supplicant/wpa_supplicant.conf -Dwext -B dhclient

	ifconfig eth1 192.168.0.251
	sleep 1
	echo -e "\nTo check the wifi port (eth1), please enter the command \"ping 192.168.0.251\" on the host PC\n"
	echo -e "\nTo restart the WPA Supplicant, before entering the command \"sh init_wpa.sh\", or to stop it, run \"sh stop_wpa.sh\"\n"

else #[ $# -eq 2 ]

	echo -e "\n$0: Bad arguments number\nUsage: $0 SSID PASSPHRASE\n"

fi

