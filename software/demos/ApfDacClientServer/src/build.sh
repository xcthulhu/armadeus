#!/bin/bash

export INITIALPATH=$PATH
. ../../../../armadeus_env.sh
ARMADEUS_QTE_DIR=$ARMADEUS_BUILD_DIR/qt-embedded-linux-opensource-src-4.4.3

if [ -d "$ARMADEUS_QTE_DIR" ]; then
	echo "building the demos for 3 targets......"
else
	echo "Please set the ARMADEUS_BUILD_DIR variable to a correct path (currently it's: $ARMADEUS_BUILD_DIR) or verify your Qt installation"
	exit 1
fi

buildForEnvironment()
{
	echo
	echo "********** Building the Network protocol library for $1 **********"
	echo

	cd ./Networking
	qmake ApfNetwork.pro
	
	if [ $? -ne 0 ]; then
		exit -1;
	fi
	make $2
	cd -

	echo
	echo "********** Building the Widget library for $1 **********"
	echo

	cd ./widgets
	qmake widgets.pro
	
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


# build for Host

export OUTPUT_DIR=../../bin/linux
#export QTDIR=/usr/local/Trolltech/Qt-4.2.0/
export QTDIR=/usr/share/qt4
# export QMAKESPEC=$QTDIR/mkspecs/linux-g++
export QMAKEPATH=$QTDIR
export PATH=$QTDIR/bin:$INITIALPATH
buildForEnvironment "Host" $1


# build for APF

export OUTPUT_DIR=../../bin/apf
#export QTDIR=$ARMADEUS_ROOTFS_DIR/qt/qtopia.4.2.0/
export QTDIR=$ARMADEUS_QTE_DIR
## due to some installation problems.... I use a custom qmake spec
export QMAKESPEC=../../QtopiaMkspec/linux-arm-g++/
export PATH=$ARMADEUS_TOOLCHAIN_PATH:$QTDIR/bin:$INITIALPATH

buildForEnvironment "APF board" $1


# build for Host with virtual framebuffer (qvfb)

export OUTPUT_DIR=../../bin/qvbf
#export QTDIR=$ARMADEUS_ROOTFS_DIR/qt/qvfb/
export QTDIR=/usr/local/Trolltech/QtEmbedded-4.4.1/
export QMAKESPEC=$QTDIR/mkspecs/linux-g++
export QMAKEPATH=$QTDIR
export PATH=$QTDIR/bin:$INITIALPATH

buildForEnvironment "Virtual Frame Buffer" $1

exit 0

