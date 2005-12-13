#!/bin/bash

ARCHIVE_LOCATION="http://members.chello.be/cr26864/Linux/fbdev/"
PACKAGE_NAME="fbset-2.1"
ARCHIVE_NAME=$PACKAGE_NAME".tar.gz"
PATCH_FILE="fbset.patch"


wget $ARCHIVE_LOCATION/$ARCHIVE_NAME

if [ -f $ARCHIVE_NAME ]; then
  tar zxf $ARCHIVE_NAME

  cd $PACKAGE_NAME/
  patch -p1 < ../$PATCH_FILE
  make ARCH=arm CROSS_COMPILE=arm-linux-

  cd ..
  rm $ARCHIVE_NAME

  echo
  echo "FBSET tool installed !"
fi
