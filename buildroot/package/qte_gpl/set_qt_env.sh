#!/bin/bash

#
# Environment variables to source when one wants to use Qt/E, Opie, Qtopia & co
#

export QTDIR=/mnt/host/usr
export OPIEDIR=/mnt/host/usr
export QPEDIR="$OPIEDIR"

export PATH=$QTDIR/bin:$QPEDIR/bin:$PATH
export LD_LIBRARY_PATH=$QTDIR/lib:$QPEDIR/lib:$LD_LIBRARY_PATH
export QT_THREAD_SUFFIX=-mt

