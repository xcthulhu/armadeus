#!/bin/sh

#
# Script to test Armadeus boards performances to handle MMC/SD cards
#
#  Copyright (C) 2007 The Armadeus Project
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

# Script parameters
MMC_MOUNT_DIR="/media/mmc"
MMC_DEVICE="/dev/mmcblk0p1"
TEMP_DIR="/tmp/perf_mmc/"
TEMP_FILE="/tmp/data.bin"
TEMP_FILE_SIZE=4096 # kbytes
WRITE_BENCH="/tmp/test_mmc.sh"

WTIME=0
RTIME=0
#DEBUG=True

debug()
{
    if [ "$DEBUG" == "True" ]; then
        echo $1
    fi
}

get_time_in_ms_from_file()
{
    if [ -f "$1" ]; then
        TIME=`cat $1 | grep real | cut -d m -f 2 | sed 's/s//'`
        STIME=`echo $TIME | cut -d . -f 1`
        STIME=`expr $STIME \* 1000`
        MSTIME=`echo $TIME | cut -d . -f 2`
        MSTIME=`expr $MSTIME \* 10`
        NAME=`echo $1 | grep write`
        if [ "$NAME" != "" ]; then
            WTIME=`expr $STIME + $MSTIME`
            debug "WTIME: $WTIME"
        else
            RTIME=`expr $STIME + $MSTIME`
            debug "RTIME: $RTIME"
        fi
    else
        echo "File doesn't exist"
    fi
}

test_speed()
{
    # Create temp bench file
    echo "Creating temp bench file"
    if [ ! -f "$TEMP_FILE" ]; then
        dd if=/dev/urandom of=/tmp/data.bin bs=1024 count=$TEMP_FILE_SIZE
    fi
    # Create bench script
    echo "mv $TEMP_FILE $MMC_MOUNT_DIR" > $WRITE_BENCH
    echo "sync" >> $WRITE_BENCH
    echo "exit 0" >> $WRITE_BENCH

    for it in 1 2 3 4 5; do
        echo "--- Test iteration n°$it ---"
        echo "  Writing"
        time sh $WRITE_BENCH 2>/tmp/writetime
        umount $MMC_MOUNT_DIR && mount $MMC_DEVICE $MMC_MOUNT_DIR
        echo "  Reading"
        time mv $MMC_MOUNT_DIR/data.bin /tmp/ 2>/tmp/readtime
        # Update writing mean time
        get_time_in_ms_from_file /tmp/writetime
        if [ $it == "1" ]; then
            WTOTIME=$WTIME
        else
            WTOTIME=`expr $WTIME + $WTOTIME`
            WTOTIME=`expr $WTOTIME / 2`
        fi
        debug "Mean write time: $WTOTIME ms"
        # Update reading mean time
        get_time_in_ms_from_file /tmp/readtime
        if [ $it == "1" ]; then
            RTOTIME=$RTIME
        else
            RTOTIME=`expr $RTIME + $RTOTIME`
            RTOTIME=`expr $RTOTIME / 2`
        fi
        debug "Mean reading time: $RTOTIME ms"
    done
    WSPEED=`expr $TEMP_FILE_SIZE \* 1000 / $WTOTIME`
    RSPEED=`expr $TEMP_FILE_SIZE \* 1000 / $RTOTIME`
    echo "--- Test result (mean values): read -> $RSPEED kBytes/sec   write -> $WSPEED kBytes/sec"
}

# Check some prerequisites
if [ ! -d $MMC_MOUNT_DIR ]; then
    echo "Check that $MMC_MOUNT_DIR directory is existing"
    exit 1
fi
IS_MOUNTED=`mount | grep -c $MMC_MOUNT_DIR`
if [ "$IS_MOUNTED" != "1" ]; then
    echo "Check that your MMC is mounted on $MMC_MOUNT_DIR"
    exit 1
fi

# Process parameters
if [ $# == 0 ]; then
    echo -e "\n   Usage: $0 [speed|integrity] \n"
    exit 1
fi

if [ "$1" == "speed" ]; then
    test_speed
    exit 0
fi


SIZE=`df -hm | grep /media/mmc | awk '{$1=$1;print}' | cut -d " " -f 4`

SIZE=40
it=0
mkdir -p $TEMP_DIR

date
FILE=$TEMP_DIR/test.rnd
MD5=$TEMP_DIR/test.rnd.md5
dd if=/dev/urandom of=$FILE bs=1024 count=1024 2>/dev/null
md5sum $FILE > $MD5

while [ $it -le $SIZE ]; do
	FILEMMC=$MMC_MOUNT_DIR/test_$it.rnd
#	MD5=$TEMP_DIR/test_$it.rnd.md5
	echo -en "\r Creating: $FILEMMC"
	it=$((it+1))
	cp $FILE $FILEMMC
	sync
done

echo ""

umount $MMC_MOUNT_DIR
mount $MMC_DEVICE $MMC_MOUNT_DIR

date
FILES=`ls $MMC_MOUNT_DIR/*.rnd`
for file in $FILES; do
	file=`basename $file`
	echo -en "\r Checking MD5 for: $file"
	MD5MMC=`md5sum $MMC_MOUNT_DIR/$file | cut -d " " -f 1`
	MD5ORG=`cat $MD5 | cut -d " " -f 1`
	if [ "$MD5MMC" != "$MD5ORG" ]; then
		echo -e "\nMD5 failed for $file: $MD5MMC $MD5ORG"
		exit 1
	fi
done
echo
date
echo "MMC perf test successful !"
rm -rf $TEMP_DIR
rm $MMC_MOUNT_DIR/*.rnd

exit 0
