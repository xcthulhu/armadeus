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

#!/bin/sh

if [ "$1" == "" ]; then
	echo "Please provides the directory where to install the PPS Test scripts"
	echo "$0 dest_dir"
	exit 1
fi

THIS_DIR=`dirname $0`
TESTSCRIPTS_TARGET_DIR="$1"
echo "Installing PPS Test scripts in $TESTSCRIPTS_TARGET_DIR (from $THIS_DIR)"

mkdir -p $TESTSCRIPTS_TARGET_DIR

install $THIS_DIR/test_*.sh $TESTSCRIPTS_TARGET_DIR/
install $THIS_DIR/init_*.sh $TESTSCRIPTS_TARGET_DIR/
install $THIS_DIR/reset_wi2wi.sh $TESTSCRIPTS_TARGET_DIR/
install $THIS_DIR/stop_wpa.sh $TESTSCRIPTS_TARGET_DIR/

exit 0
