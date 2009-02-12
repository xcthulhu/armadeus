#!/bin/bash

show_usage()
{
    echo
    echo "Move (svn mv) patches from given version dir to new version:"
    echo "    $0 old_version_dir new_version"
    echo
    exit 1
}

if [ $# != 2 ]; then
	show_usage
fi

if [ ! -d "$1" ]; then
	echo "$1 source directory doesn't exist"
	show_usage
fi

if [ -d "$2" ]; then
	echo "$2 directory is already existing !!"
	show_usage
fi

svn mv $1 $2

echo "********"
echo "I'm now going to commit the renaming of $1 to $2 (first step of the patches moving)"
echo "ARE YOU SURE YOU WANT TO PROCEED ??"
echo "    y/n"
read answer

if [ "$answer" != "y" ]; then
	svn revert $2
	svn revert -R $1
	rm -rf $2
	echo " Nothing changed !!"
	exit 1
fi

svn commit -m "[LINUX] Moves $1 patches to $2 (first step)"

# Rename patches
PATCHES=`ls $2 | grep $1`
for patch in $PATCHES; do
	new_name=`echo $patch | sed -e s/$1/$2/`
	echo "renaming $1/$patch in $2/$new_name"
	svn mv $2/$patch $2/$new_name
done

echo "Don't forget to move Linux configuration in Buildroot too !!"

# Bye bye
exit 0

