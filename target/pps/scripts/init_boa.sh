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

# setup /etc/hosts file (hosname id required by "boa")
echo "127.0.0.1       localhost" > /etc/hosts
echo "192.168.0.250   armadeus"  >> /etc/hosts

if [ -d "/var/www" ]; then
	kill -9 `pidof boa` 2> /dev/null
	mkdir /var/log/boa 2>/dev/null 
	boa
fi

