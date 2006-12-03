#!/bin/bash

export INITIALPATH=$PATH

if   [  -z $ARMADEUS_ROOT_DIR ]; then
	cd ../../../../..
	export  ARMADEUS_ROOT_DIR=`pwd`
	cd -
	if   [  -d $ARMADEUS_ROOT_DIR/qt/ ]; then
		echo "building the demos for 3 targets......"
	else
		echo "Please set the $ARMADEUS_ROOT_DIR variable to correct variable or verify the qt installations"
		exit 1
	fi
fi

buildForEnvironment()
{


	echo
	echo "********** Building the Network protocol library for $1 **********"
	echo
	echo 

	cd ./Networking
	qmake ApfNetwork.pro
	
	if [ $? -ne 0 ]; then
		exit -1;
	fi
	make $2
	cd -

	echo
	echo "********** Building the Server applications for $1 **********"
	echo 
	
	cd ./Server
	qmake Server.pro
	if [ $? -ne 0 ]; then
		exit -1;
	fi
	make $2 

	qmake ServerNoGui.pro
	if [ $? -ne 0 ]; then
		exit -1;
	fi
	make $2
	cd -

	echo
	echo "********** Building the Client application for $1 **********"
	echo
	
	cd ./Client
	qmake ApfClient.pro
	if [ $? -ne 0 ]; then
		exit -1
	fi
	make $2
	cd -
}


# build for linux

export OUTPUT_DIR=../../bin/linux
export QTDIR=/usr/local/Trolltech/Qt-4.2.0/
# export QMAKESPEC=$QTDIR/mkspecs/linux-g++
export QMAKEPATH=$QTDIR
export PATH=$QTDIR/bin:$INITIALPATH
buildForEnvironment "Host" $1


# build for embedded

export OUTPUT_DIR=../../bin/apf
export QTDIR=$ARMADEUS_ROOT_DIR/qt/qtopia.4.2.0/

## due to some installation problems.... I use a custom qmake spec
export QMAKESPEC=../../QtopiaMkspec/linux-arm-g++/
export PATH=$QTDIR/bin:$INITIALPATH

buildForEnvironment "Apf board" $1

# build for frame buffer

export OUTPUT_DIR=../../bin/qvbf
export QTDIR=$ARMADEUS_ROOT_DIR/qt/qvfb/
export QMAKESPEC=$QTDIR/mkspecs/linux-g++
export QMAKEPATH=$QTDIR
export PATH=$QTDIR/bin:$INITIALPATH

buildForEnvironment "Frame Buffer" $1

exit 0
