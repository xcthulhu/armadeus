#!/bin/bash

ARCHIVE_LOCATION="http://www.pengutronix.de/software/ptxdist/temporary-src/"
PACKAGE_NAME="fbtest-20041102-1"
ARCHIVE_NAME=$PACKAGE_NAME".tar.gz"
PATCH_FILE="fbtest.patch"


wget $ARCHIVE_LOCATION/$ARCHIVE_NAME

if [ -f $ARCHIVE_NAME ]; then
  tar zxf $ARCHIVE_NAME

  cd $PACKAGE_NAME/
  patch -p1 < ../$PATCH_FILE
  make ARCH=arm CROSS_COMPILE=arm-linux-

  cd ..
  rm $ARCHIVE_NAME

  echo
  echo "FBTEST tool installed !"
fi
