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

if [ $# -eq 1 ]; then

	if [ -f /etc/ipsec-tools.conf ]; then

		#****** 1: definition ***** 

		#dont forget to edit the file /etc/ipsec-tools.conf on the system with this address
		export IPADDR=$1

		#****** 2:create connection (PPS Ethernet port) ****
		ifconfig usb0   down 2>/dev/null
		ifconfig eth1   down 2>/dev/null
		ifconfig usb1   down 2>/dev/null
	
		ifconfig eth0 $IPADDR

		#****** 3:securize the connection with IPSec ****
		setkey -f /etc/ipsec-tools.conf

		if [ $? -eq 0 ]; then
			echo -e "\nHere are the keys you just set with IPSec:\n"
			setkey -D
			
			echo -e "\nNow you can test the IPSec securized connection by activating it on your host PC:\n- First you have to put on your host PC in the directory /etc/ the file ipsec-tools.conf.\n- Then you can run IPSec with the command: sudo /etc/init.d/setkey start\n- On your system, you can run a telnet toward your host PC\n- Check with Wireshark on your host PC that you can't see the protocol TELNET and your password in the datagrams.\n"	
		fi 
	else #[ -f /etc/ipsec-tools.conf ]
		echo -e '\nThe file /etc/ipsec-tools.conf doesnt exist.\nYou have to create this file and fill it with the correct datas, like this:\n
		## Flush the SAD and SPD
		#
		flush;
		spdflush;

		# Attention: Use this keys only for testing purposes!
		# Generate your own keys!

		# AH SAs using 128 bit long keys
		add 192.168.0.225 192.168.0.208	ah 0x200 -A hmac-md5
			0xc0291ff014dccdd03874d9e8e4cdf3e6;

		add  192.168.0.208 192.168.0.225 ah 0x300 -A hmac-md5
			0x96358c90783bbfa3d7b196ceabe0536b;

		# ESP SAs using 192 bit long keys (168 + 24 parity)

		add 192.168.0.225 192.168.0.208	esp 0x201 -E 3des-cbc
			0x7aeaca3f87d060a12f4a4487d5a5c3355920fae69a96c831;
		add 192.168.0.208 192.168.0.225	esp 0x301 -E 3des-cbc
			0xf6ddb555acfd9d77b03ea3843f2653255afe8eb5573965df;


		# Security policies
		spdadd 192.168.0.225 192.168.0.208 any -P out ipsec
			   esp/transport//require
			   ah/transport//require;

		spdadd 192.168.0.208 192.168.0.225 any -P in ipsec
			   esp/transport//require
			   ah/transport//require;\n'
	fi
else #[ $# -eq 1 ]
	echo -e "\n$0: Bad arguments number\nUsage: $0 IPADDRESS\n"
fi
	
