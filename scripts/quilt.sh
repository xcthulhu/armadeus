#!/bin/bash

make shell_env
. armadeus_env.sh

echo -e "\nThis script is going to rebuilt a quiltified Linux kernel in the current view...\n"

if [ "$1" == "export" ]; then
	echo "Exporting your work (patches) from $ARMADEUS_LINUX_DIR/patches/ to $ARMADEUS_LINUX_PATCH_DIR"
	cp -f $ARMADEUS_LINUX_DIR/patches/*.patch $ARMADEUS_LINUX_PATCH_DIR/
	echo -e "\n--- You can now check your work before commiting in $ARMADEUS_LINUX_PATCH_DIR/ \n"
	exit 0
fi


# else import patches:

# Update repository
echo "Update (pull) your local repository (y/N) ?"
read response
if [ "$response" == "y" ] || [ "$response" == "yes" ]; then
	git pull
fi

# Move or delete current Linux dir
echo "Rename or delete the current Linux directory: $ARMADEUS_LINUX_DIR"
echo " ?? (R/d)"
read response
EXT=`date +%Y_%M_%d_%H%m`
if [ "$response" == "d" ]; then
	echo "Deleting current Linux dir"
	rm -rf $ARMADEUS_LINUX_DIR
else
	echo "Renaming $ARMADEUS_LINUX_DIR"
	echo "  to " "$ARMADEUS_LINUX_DIR"."$EXT"
	mv $ARMADEUS_LINUX_DIR "$ARMADEUS_LINUX_DIR"."$EXT"
fi

# Get Linux sources
make linux26-unpacked

pushd $ARMADEUS_LINUX_DIR
mkdir patches

PATCHES=`ls -ar $ARMADEUS_LINUX_PATCH_DIR/*.patch`

for patch in $PATCHES; do
	quilt import $patch
done

quilt push -a
if [ "$?" != 0 ]; then
	echo "Please correct that"
	exit 1
fi

quilt series >> .applied_patches_list

touch .unpacked
touch .patched
touch .patched.arch
touch .patched.board

echo -e "\n--- Your kernel is \"quiltified\" ! I will now compile it...\n"
sleep 2

popd
make

echo -e "\n--- Your can now go to $ARMADEUS_LINUX_DIR\nHappy hacking ! ;-)\n"
echo -e "__ Don't forget to do a \"$0 export\" after your modifications __\n"

exit 0

