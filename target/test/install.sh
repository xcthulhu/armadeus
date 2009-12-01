#!/bin/sh

if [ "$1" == "" ]; then
	echo "Please provides the directory where to install the Test Suite"
	echo "$0 dest_dir"
	exit 1
fi

THIS_DIR=`dirname $0`
TESTSUITE_TARGET_DIR="$1"
echo "Installing Test Suite in $TESTSUITE_TARGET_DIR (from $THIS_DIR)"

mkdir -p $TESTSUITE_TARGET_DIR
mkdir -p $TESTSUITE_TARGET_DIR/packages
install $THIS_DIR/test_*.sh $TESTSUITE_TARGET_DIR/
install $THIS_DIR/dhcp.sh $TESTSUITE_TARGET_DIR/
install $THIS_DIR/mmc/*.sh $TESTSUITE_TARGET_DIR/
#       find $(ARMADEUS-TESTSUITE_DIR) -name "*.sh" -exec install -D {} $(ARMADEUS-TESTSUITE_TARGET_DIR) \;
install $THIS_DIR/packages/*.sh $TESTSUITE_TARGET_DIR/packages
cp -r $THIS_DIR/data $TESTSUITE_TARGET_DIR

exit 0
