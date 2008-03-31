#!/bin/bash

show_usage()
{
    echo
    echo "Copy existing patches and rename them for new version:"
    echo "    $0 old_version new_version"
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

# Copy dir
cp -r $1 $2

# Rename patches
FILES=`ls $1 | grep $1`
for patches in $FILES; do
    new_name=`echo $patches | sed -e s/$1/$2/`
    #echo "renaming $1/$patches in $2/$new_name"
    mv $2/$patches $2/$new_name
done
rm -rf $2/$new_name/.svn

# Bye bye
exit 0
