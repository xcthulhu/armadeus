#!/bin/bash

TEMP_DIR=./tmp_log
SVN_LOG=$TEMP_DIR/svn.log
SVN_LOG_CLEAN=$TEMP_DIR/svn_clean.log
LOG_CLEAN_TMP=$TEMP_DIR/svn_clean_tmp.log
CHANGES_LINUX=$TEMP_DIR/linux.log
CHANGES_UBOOT=$TEMP_DIR/uboot.log
CHANGES_BUILDROOT=$TEMP_DIR/buildroot.log
CHANGES_OTHER=$TEMP_DIR/other.log


usage()
{
	echo
	echo "$0 rev1 rev2: generates ChangeLog file for commit between rev1 and rev2 (included)"
	echo
}

if [ "$#" != 2 ]; then
	usage
	exit 1
fi

mkdir -p $TEMP_DIR 
echo "Generating ChangeLog for revisions between $1 and $2 in" `pwd`

if [ ! -f $SVN_LOG ]; then
	svn log -r $2:$1 > $SVN_LOG
fi
# remove unneeded lines:
cat $SVN_LOG | grep -v -e "-\{5,\}" | grep -v -e "^r[0-9]\{2,\}" > $SVN_LOG_CLEAN

# separate sections:
cat $SVN_LOG_CLEAN | grep -i  "\[LINUX\]" > $CHANGES_LINUX
cat $SVN_LOG_CLEAN | grep -iv "\[LINUX\]" > $LOG_CLEAN_TMP
cat $LOG_CLEAN_TMP | grep -i  "\[U[-]\{0,\}BOOT\]" > $CHANGES_UBOOT
cat $LOG_CLEAN_TMP | grep -iv "\[U[-]\{0,\}BOOT\]" > $LOG_CLEAN_TMP.2
cat $LOG_CLEAN_TMP.2 | grep -i  "\[BUILDROOT\]" > $CHANGES_BUILDROOT
cat $LOG_CLEAN_TMP.2 | grep -iv "\[BUILDROOT\]" > $LOG_CLEAN_TMP.3
cat $LOG_CLEAN_TMP.3 > $CHANGES_OTHER

# compose changelog with sections without empty lines:

echo
echo "Changes in release XXX ("`date`" - SVN revisions $1 to $2"
echo
echo "* Buildroot:"
cat $CHANGES_BUILDROOT | grep -v "^$" | sed 's/\[[Bb][Uu][Ii][Ll][Dd][Rr][Oo][Oo][Tt]\]/  -/g'
echo "* Linux:"
cat $CHANGES_LINUX | grep -v "^$" | sed 's/\[[Ll][Ii][Nn][Uu][Xx]\]/  -/g'
echo "* U-Boot:"
cat $CHANGES_UBOOT | grep -v "^$" | sed 's/\[[Uu][-]\{0,\}[Bb][Oo][Oo][Tt]\]/   -/g'
echo "* Other:"
cat $CHANGES_OTHER | grep -v "^$"

rm -rf $TEMP_DIR

exit 0

