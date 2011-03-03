#!/bin/bash

TEMP_DIR=./tmp_log
SVN_LOG=$TEMP_DIR/svn.log
SVN_LOG_CLEAN=$TEMP_DIR/svn_clean.log
LOG_CLEAN_TMP=$TEMP_DIR/svn_clean_tmp.log
CHANGES_LINUX=$TEMP_DIR/linux.log
CHANGES_UBOOT=$TEMP_DIR/uboot.log
CHANGES_BUILDROOT=$TEMP_DIR/buildroot.log
CHANGES_FIRMWARE=$TEMP_DIR/firmware.log
CHANGES_TEST=$TEMP_DIR/test.log
CHANGES_DEMOS=$TEMP_DIR/demos.log
CHANGES_DEBUG=$TEMP_DIR/debug.log
CHANGES_TOOLS=$TEMP_DIR/tools.log
CHANGES_ASDEVICES=$TEMP_DIR/asdevices.log
CHANGES_OTHER=$TEMP_DIR/other.log

LATEST_RELEASE_DATE="2010-07-30"

usage()
{
	echo
	echo "$0: generates ChangeLog file for commit since latest release"
	echo
}

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
	usage
	exit 1
fi

# get log
mkdir -p $TEMP_DIR 
echo "Generating ChangeLog for commit since $LATEST_RELEASE_DATE in" `pwd`
if [ ! -f $SVN_LOG ]; then
	git log --since="$LATEST_RELEASE_DATE" --no-color --pretty=oneline > $SVN_LOG
fi

# remove commit hash number:
cat $SVN_LOG | cut -c 42- > $SVN_LOG_CLEAN

# separate sections:
cat $SVN_LOG_CLEAN | grep -i  "\[LINUX\]" > $CHANGES_LINUX
cat $SVN_LOG_CLEAN | grep -iv "\[LINUX\]" > $LOG_CLEAN_TMP
cat $LOG_CLEAN_TMP | grep -i  "\[U[-]\{0,\}BOOT\]" > $CHANGES_UBOOT
cat $LOG_CLEAN_TMP | grep -iv "\[U[-]\{0,\}BOOT\]" > $LOG_CLEAN_TMP.2
cat $LOG_CLEAN_TMP.2 | grep -i  "\[BUILDROOT\]" > $CHANGES_BUILDROOT
cat $LOG_CLEAN_TMP.2 | grep -iv "\[BUILDROOT\]" > $LOG_CLEAN_TMP.3
cat $LOG_CLEAN_TMP.3 | grep -i  "\[FIRMWARE\]" > $CHANGES_FIRMWARE
cat $LOG_CLEAN_TMP.3 | grep -iv "\[FIRMWARE\]" > $LOG_CLEAN_TMP.4
cat $LOG_CLEAN_TMP.4 | grep -i  "\[TEST\]" > $CHANGES_TEST
cat $LOG_CLEAN_TMP.4 | grep -iv "\[TEST\]" > $LOG_CLEAN_TMP.5
cat $LOG_CLEAN_TMP.5 | grep -i  "\[DEMOS\]" > $CHANGES_DEMOS
cat $LOG_CLEAN_TMP.5 | grep -iv "\[DEMOS\]" > $LOG_CLEAN_TMP.6
cat $LOG_CLEAN_TMP.6 | grep -i  "\[DEBUG\]" > $CHANGES_DEBUG
cat $LOG_CLEAN_TMP.6 | grep -iv "\[DEBUG\]" > $LOG_CLEAN_TMP.7
cat $LOG_CLEAN_TMP.7 | grep -i  "\[TOOLS\]" > $CHANGES_TOOLS
cat $LOG_CLEAN_TMP.7 | grep -iv "\[TOOLS\]" > $LOG_CLEAN_TMP.8
cat $LOG_CLEAN_TMP.8 | grep -i  "\[AS_DEVICES\]" > $CHANGES_ASDEVICES
cat $LOG_CLEAN_TMP.8 | grep -iv "\[AS_DEVICES\]" > $LOG_CLEAN_TMP.9

cat $LOG_CLEAN_TMP.9 > $CHANGES_OTHER

# compose Changelog:
echo
echo "----"
echo
echo "Changes in release XXX ("`date`")"
echo
echo "* Buildroot:"
cat $CHANGES_BUILDROOT | grep -v "^$" | sed 's/\[[Bb][Uu][Ii][Ll][Dd][Rr][Oo][Oo][Tt]\]/    -/g'
echo "* Linux:"
cat $CHANGES_LINUX | grep -v "^$" | sed 's/\[[Ll][Ii][Nn][Uu][Xx]\]/    -/g'
echo "* U-Boot:"
cat $CHANGES_UBOOT | grep -v "^$" | sed 's/\[[Uu][-]\{0,\}[Bb][Oo][Oo][Tt]\]/    -/g'
echo "* Firmware:"
cat $CHANGES_FIRMWARE | grep -v "^$" | sed 's/\[[Ff][Ii][Rr][Mm][Ww][Aa][Rr][Ee]\]/    -/g'
echo "* Demos:"
cat $CHANGES_DEMOS | grep -v "^$" | sed 's/\[DEMOS\]/    -/g'
echo "* Debug:"
cat $CHANGES_DEBUG | grep -v "^$" | sed 's/\[DEBUG\]/    -/g'
echo "* Test:"
cat $CHANGES_TEST | grep -v "^$" | sed 's/\[[Tt][Ee][Ss][Tt]\]/    -/g'
echo "* Tools:"
cat $CHANGES_TOOLS | grep -v "^$" | sed 's/\[[Tt][Oo][Oo][Ll][Ss]\]/    -/g'
echo "* As_Devices:"
cat $CHANGES_ASDEVICES | grep -v "^$" | sed 's/\[[Aa][Ss]_[Dd][Ee][Vv][Ii][Cc][Ee][Ss]\]/    -/g'
echo "* Other:"
cat $CHANGES_OTHER | grep -v "^$"
echo

# remove temp stuff
rm -rf $TEMP_DIR

exit 0

