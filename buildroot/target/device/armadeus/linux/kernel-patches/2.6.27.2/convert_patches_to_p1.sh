#!/bin/bash

show_usage()
{
	echo
	echo "Convert p0 pengutronix patches to p1 (needed by Buildroot):"
	echo "    $0 patches"
	echo
	exit 1
}

echo "$# files will be modified:"
if [ $# -lt 1 ]; then
	show_usage
fi

while [ "$1" != "" ]; do
	if [ ! -f "$1" ]; then
		echo "$1 not a valid file"
	fi
	echo "    $1"
	cat "$1" | sed -e "s/^--- /--- linux\//" | sed -e "s/^+++ /+++ linux\//" | sed -e "s/linux\/\/dev\/null/\/dev\/null/" > "$1".new
	mv "$1".new "$1"
	shift 1
done

# Bye bye
exit 0
