#!/bin/sh
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

BUILDROOT_NAME=buildroot
BUILDROOT_VERSION=-20051129

INSTALL_DIR=`pwd`/software
PATCH_DIR=`pwd`/software/patchs
BUILD_DIR=$INSTALL_DIR
DL_DIR=$INSTALL_DIR/downloads                    # do not modify!!
TAR_OPTIONS=-xvf

BUILDROOT_FILE=$BUILDROOT_NAME$BUILDROOT_VERSION.tar.bz2
BUILDROOT_WEBSITE=http://buildroot.uclibc.org/downloads/snapshots/$BUILDROOT_FILE
BUILDROOT_PATCH_FILE=buildroot.patch

abort() {
    echo armadeus: $@
    exec false
}

testAndDownload()
{
    if [ ! -e $DL_DIR/$1 ]; then
    wget --tries=5 -P $DL_DIR $2
        if [ $? == 1 ]; then 
            abort unable to get $1 from website $2
        fi
    fi
    echo $1 already present in $DL_DIR so take it!!
}

logresult()
{
    if [ $1 == 1 ]; then 
        abort $2 failed
    fi
}

echo "******* create download folder if required ******* "
cd $INSTALL_DIR
if [ ! -d $DL_DIR ]; then
mkdir $DL_DIR
fi

echo "************* Downloading buildroot ************* "
testAndDownload $BUILDROOT_FILE $BUILDROOT_WEBSITE

echo "************* Uncompressing buildroot ************* "
bzcat $DL_DIR/$BUILDROOT_FILE | tar -C $BUILD_DIR $TAR_OPTIONS -
cd $BUILD_DIR/$BUILDROOT_NAME

echo "************* patching buildroot ************* "
patch -p1 < $PATCH_DIR/$BUILDROOT_PATCH_FILE
# chmod +x $BUILD_DIR/$BUILDROOT_NAME/target/generic/target_skeleton/etc/init.d/S18pormap

echo "********* Building buildroot !! Takes several hours !!! ************* "
make
make
logresult $? "buildroot install failed"

echo "***************** Building done !! That's all folks !!! ************* "

