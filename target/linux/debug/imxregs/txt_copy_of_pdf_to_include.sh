#!/bin/bash

FILENAME=$1

# Remove multiple whitspaces
cat $FILENAME | sed "s/[[:space:]]\{1,\}/ /g" > $FILENAME.1

echo "Please manually adjust $FILENAME.1"
read

awk '{ print "// "$1" { \""$3"\", "$2", 0, 0xffffffff, x, \""$4" "$5" "$6" "$7" "$8" "$9"\" }," }' $FILENAME.1 > $FILENAME.2
echo "Please manually adjust $FILENAME.2"
read

cat $FILENAME.2 | sed "s/[[:space:]]\{2,\}//g" > $FILENAME.3
cat $FILENAME.3 | sed "s/ \" }/\" }/g" > $FILENAME.4
echo "Please manually adjust $FILENAME.4"
read



