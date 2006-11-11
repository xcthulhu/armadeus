
#
#   THE ARMADEUS PROJECT
#
#
#   README file for compiling the LCD4linux package for APM9328 boards
#
#

#   Kernel configuration to enable usage of LCD4linux :
#
#   You must have "gpio" and "ppdev" modules loaded in your kernel
#   /dev/parport/0 should have been "mknod-ed"
#   


Automatic compiling:
-------------------

-> make lcd4linux
Files will be installated in /local/export  (your NFS export dir)

Manually compiling:
-------------------

-> download package from http://sourceforge.net/project/showfiles.php?group_id=1310 ( tested with version 0.10.0 )

-> extract archive: tar zxvf lcd4linux-0.10.0.tar.gz

-> copy "patch-lcd4linux-armadeus" into generated directory "lcd4linux-0.10.0/"

-> cd lcd4linux-0.10.0/

-> apply patch: patch -p1 < patch-lcd4linux-armadeus

-> ./configure CC=arm-linux-gcc --host=arm-linux

-> make

-> generated binary is "lcd4linux"

-> strip it to reduce it's size: arm-linux-strip -s lcd4linux


Tested with:
--------------

host:           FEDORA CORE 3  
cross compiler: gcc version 3.4.2


Installation:
-------------

1) Copy the "lcd4linux" binaries into /usr/bin/
2) Copy the "lcd4linux_armadeus.conf" into /etc/ and rename it "lcd4linux.conf"
4) Insmod "gpio" and "ppdev" modules


Typical Usage :
--------------

# /usr/bin/lcd4linux


Typical problems:
-----------------

TBDL

See lcd4linux documentation for more details
See Armadeus Wiki for uptodate informations !!


Have fun !

Julien


