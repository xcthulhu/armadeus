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
