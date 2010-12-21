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

#****** 1: definition ***** 

export USERNAME=
export HOSTNAME=
#dont forget to add the tunnel port to the file /etc/ssh/sshd_config on your Host PC.
export PORTNAME= #(configure firewall and NAT for this port if necessary) 
# define tunnel id (port;standard value should be 23; avoid it because NAT config may be already used for ssh ) 
export TELNETPORT=

if [ "$USERNAME" != "" -a "$HOSTNAME" != "" -a "$PORTNAME" != "" -a "$TELNETPORT" != "" ]; then
	# ****** 2: Activate the connection interface (PPS Ethernet port) *****
	ifconfig usb0   down 2>/dev/null
	ifconfig eth1   down 2>/dev/null
	ifconfig usb1   down 2>/dev/null
	
	ifconfig eth0 192.168.0.208

	# ****** 2: create the ssh tunnel (check first if exists !!!)***** 

	ps -eaf |grep "ssh -fN -L $TELNETPORT:localhost:$TELNETPORT -C $USERNAME@$HOSTNAME -p $PORTNAME" | grep -vq "grep" 
	[ $? -eq 1 ] && ssh -fN -L $TELNETPORT:localhost:$TELNETPORT -C $USERNAME@$HOSTNAME -p $PORTNAME ; 

	# *** 3: start telnet (if tunnel exist!!) : !! NOTE: host name is localhost !!! *** 

	ps -eaf |grep "ssh -fN -L $TELNETPORT:localhost:$TELNETPORT -C $USERNAME@$HOSTNAME -p $PORTNAME" | grep -vq "grep" 
	res=$?

	[ $res -eq 0 ] && echo -e "\nSSH tunnel is created on localhost, port 23. Now connecting to host $HOSTNAME with telnet.\n You can check if the tunnel is properly encrypting the datas with Wireshark: you must not see the protocol TELNET or your password in the datagrams.\n"
	[ $res -eq 0 ] &&  telnet -l $USERNAME  localhost 
else
	echo -e "\nOne or many of the variables USERNAME, HOSTNAME, PORTNAME and TELNETPORT are not initialized. \nYou have to fill these variables in the file /usr/local/pps/scripts/test_ssh_tunnel.sh\n"
fi



