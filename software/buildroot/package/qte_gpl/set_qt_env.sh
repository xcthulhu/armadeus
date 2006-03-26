#!/bin/bash

#
# Environment variables to source when one wants to use Qt/E
#

export QTDIR=/mnt/host
export PATH=$QTDIR/bin:$PATH
export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QT_THREAD_SUFFIX=-mt

