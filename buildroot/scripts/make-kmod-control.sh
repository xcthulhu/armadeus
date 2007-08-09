#!/bin/bash

#SCRIPT_DIR=`pwd`/scripts/ ./scripts/make-kmod-control.sh target/device/armadeus/linux/control/ build_arm_nofpu/linux-2.6.18.1/.config bin/packages/

MODULE_PATH_PATTERN="Location="
MODULE_CONFIG_PATTERN="Config="

ARCH=arm

debug()
{
	echo $1
}

print_usage()
{
	echo
	echo "  Usage: $0 control_files_dir linux_config package_building_dir"
	echo
	exit 0
}

CONTROL_DIR=$1
PACKAGE_DIR=$3  # pass it $(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ]; then
	print_usage
fi

# sources linux .config file
. "$2"
CONTROL_FILES=`ls $CONTROL_DIR`
mkdir -p PACKAGE_DIR

cd "$CONTROL_DIR"
TEMP_FILE="./temp.control"
for File in $CONTROL_FILES; do
	if [ -f "$File" ]; then
		debug "--- Testing package Build for $File"
		# Remove custom tag in corresponding .control file    ../control/kmod-$(2).control
		cat $File | grep -v $MODULE_PATH_PATTERN | grep -v $MODULE_CONFIG_PATTERN > $TEMP_FILE
		# create linux-modules/ipkg install dirs (even if not needed)
		$SCRIPT_DIR/make-ipkg-dir.sh $PACKAGE_DIR $TEMP_FILE 2.6.12-apf-2.2 $ARCH
		#echo "Depends: $$(IDEPEND_$(1))" >> $$(I_$(1))/CONTROL/control
		MODULE_PATH=`cat $File | grep $MODULE_PATH_PATTERN | cut -d = -f 2`
		#debug $MODULE_PATH
		CONFIG_FLAG=`grep $MODULE_CONFIG_PATTERN $File | cut -d = -f 2`
		debug $CONFIG_FLAG
		eval temp=\$$CONFIG_FLAG
		if [ "$temp" == "m" ]; then
			debug "packaging: $temp"
			# Package should be built because driver was configured as a module
		fi
	fi
done

echo "CONFIG: $CONFIG_MODULE_UNLOAD"

exit 0