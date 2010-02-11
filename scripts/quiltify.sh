#!/bin/bash
#
# "quiltifies" Linux or Buildroot
#

answer=""

ask_user_banner()
{
        echo
        echo -e "--- "$1
}

# $1: thing to ask, $* possibilities. Will fill in "answer" variable.
ask_user()
{
        ask_user_banner "$1"
        shift
        n=1
        while [ "$1" != "" ]; do
                echo "  $n] "$1
                shift
                let n=n+1
        done
        read -p "> " answer
}

ask_user_choice()
{
        ask_user_banner "$1"
        shift
	export PS3="> "
	select answer in $1; do
#		echo "($answer) ($REPLY)"
		if [ "$answer" == "" ]; then
			echo "Bad choice: $REPLY"
		else
			break
		fi
	done
}


# Get useful envt variables
make shell_env
. armadeus_env.sh

if [ "$1" != "export" ]; then
	ask_user_choice "What do you want to quiltify today ? ;-)" "Linux_(default) Buildroot U-Boot"
fi
if [ "$1" == "export" ]; then
	ask_user_choice "What kind of quilt patches do you want to export ?" "Linux_(default) Buildroot U-Boot"
fi

if [ "$answer" == "Buildroot" ]; then
	QUILT_TARGET_NAME="Buildroot"
	QUILT_MAKEFILE_TARGET="buildroot-unpacked"
	QUILT_TARGET_DIR=$ARMADEUS_BUILDROOT_DIR
	QUILT_TARGET_PATCH_DIR=$ARMADEUS_BUILDROOT_PATCH_DIR
elif [ "$answer" == "U-Boot" ]; then
	QUILT_TARGET_NAME="U-Boot"
	QUILT_MAKEFILE_TARGET="u-boot-src-unpacked"
	QUILT_MAKEFILE_TARGET_TOUCH=".unpacked .patched"
	QUILT_PATCH_FILTER="$ARMADEUS_U_BOOT_VERSION"
	QUILT_TARGET_DIR=$ARMADEUS_U_BOOT_DIR
	QUILT_TARGET_PATCH_DIR=$ARMADEUS_BUILDROOT_DIR/../patches/u-boot
else
	QUILT_TARGET_NAME="Linux kernel"
	QUILT_MAKEFILE_TARGET="linux26-patched"
	QUILT_TARGET_DIR=$ARMADEUS_LINUX_DIR
	QUILT_TARGET_PATCH_DIR=$ARMADEUS_LINUX_PATCH_DIR
fi


## Copy patches from working dir to version dir:

if [ "$1" == "export" ]; then
	echo "Exporting your work (patches) from $QUILT_TARGET_DIR/patches/ to $QUILT_TARGET_PATCH_DIR"
	cp -f $QUILT_TARGET_DIR/patches/*.patch $QUILT_TARGET_PATCH_DIR
	echo -e "\n--- You can now check your work before commiting --> \n"
	if [ -d ".git" ]; then
		git status
	elif [ -d ".svn" ]; then
		svn status -q
	else
		echo "No VCS found"
	fi
	exit 0
fi

## else import patches in working dir:

echo -e "\nThis script is going to rebuild a quiltified "$QUILT_TARGET_NAME" in the current view..."

# Update repository
ask_user "Update (pull) your local GIT repository (y/N) ?"
if [ "$answer" == "y" ] || [ "$answer" == "yes" ]; then
	git pull
fi

# Move or delete current XXX dir
if [ "$QUILT_TARGET_NAME" != "Buildroot" ]; then
	ask_user "Rename or delete the current $QUILT_TARGET_NAME directory: $QUILT_TARGET_DIR \n?? (R/d)"
	EXT=`date +%Y_%m_%d_%Hh%M`
	if [ "$answer" == "d" ]; then
		echo -n "Deleting current $QUILT_TARGET_NAME dir..."
		rm -rf $QUILT_TARGET_DIR
		echo "done"
	else
		echo "Renaming $QUILT_TARGET_DIR"
		echo "  to " "$QUILT_TARGET_DIR"."$EXT"
		mv $QUILT_TARGET_DIR "$QUILT_TARGET_DIR"."$EXT"
	fi
else # For Buildroot
	rm -f $QUILT_TARGET_DIR/.patched
	rm -f $QUILT_TARGET_DIR/.unpacked
	rm -rf $QUILT_TARGET_DIR/patches
	rm -rf $QUILT_TARGET_DIR/.pc
fi

# Get XXX unpacked sources
echo "Getting sources... (make $QUILT_MAKEFILE_TARGET)"
make $QUILT_MAKEFILE_TARGET
if [ "$?" != 0 ]; then
	echo "Failed to get sources, please check your view !!"
fi

# Import patches
pushd $QUILT_TARGET_DIR
mkdir patches
if [ "$QUILT_PATCH_FILTER" != "" ]; then
	PATCHES=`ls -ar $QUILT_TARGET_PATCH_DIR/*.patch | grep $QUILT_PATCH_FILTER`
else
	PATCHES=`ls -ar $QUILT_TARGET_PATCH_DIR/*.patch`
fi
for patch in $PATCHES; do
	quilt import $patch
done
quilt push -a
if [ "$?" != 0 ]; then
	echo "Please correct that"
	exit 1
fi
quilt series >> .applied_patches_list
if [ "$QUILT_MAKEFILE_TARGET_TOUCH" != "" ]; then
	touch $QUILT_MAKEFILE_TARGET_TOUCH
else
	touch .unpacked .patched .patched.arch .patched.board
fi
popd

# Build it
echo -e "\n--- Your "$QUILT_TARGET_NAME" is \"quiltified\" ! I will now compile it...\n"
sleep 2
make

echo -e "\n--- Your can now go to $QUILT_TARGET_DIR\nHappy hacking ! ;-)\n"
echo -e "__ Don't forget to do a \"$0 export\" after your modifications __\n"

exit 0

