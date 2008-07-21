#!/bin/bash

if [ $# -lt 2 ]; then
	echo "Usage: $0 [options] <source> <dest> <patchname>"
	echo "    options: --append"
	exit 1
fi

if [ "$1" == "--append" ]; then
	shift
	echo "PATCH AUTOMATICALLY GENERATED" >> $3
else
	echo "PATCH AUTOMATICALLY GENERATED" > $3
fi
echo "DON'T EDIT IT OR YOUR MODIFICATIONS WILL BE LOST" >> $3
echo "(Take a look at armadeus/target/linux/ directory to know how to generate it)" >> $3

diff -urN $1 $2 >> $3
exit 0
