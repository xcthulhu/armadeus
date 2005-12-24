
#
#	THE ARMADEUS PROJECT
#
#   Copyright (C) year  The source forge armadeus project team
#
#	README file for compiling the samba package for APM9328 boards
#
#

#	Kernel configuration to enable usage of SAMBA :
#
#	You must have a networking support (trivial).......
#	
#	"File systems->Network File Systems->SMB file system support" is enabled
#	"File systems->Network File Systems->SMB file system->Use a default NLS"  is enabled
#			"withs Default Remote NLS Option = cp437"
#
# 	Note that in some cases NLS default cp437 can be changed in samba configuration.. 
#
#	"File systems->Network File Systems->CIFS support" is enabled
#	in "File systems->Network File Systems->Native Language Support" enable following values:
#		Codepage 437 (United States, Canada)
#		Codepage 850 (Europe)
#		NLS ISO 8859-1 (Latin 1; Western European Languages)
#
#		optionally you may enable:
#		NLS ISO 8859-15 (Latin 9; Western European Languages with Euro)
#	Enable any other code pages or NLS if necessary (rare!)
#



Compiling:
----------


-> download package from www.samba.org (here tested with version 3.0.20 )

-> extract archive: tar fxvz samba-3.0.20.tar.gz

-> copy patch patch-samba-apm9326 into directory samba-3.0.20

-> go into samba-3.0.20 directory: cd .../samba-3.0.20

-> apply patch: patch -p1 < patch-samba-apm9326

-> go into source directory: cd .../samba-3.0.20/source

-> check environment variables $ARMADEUS_KERNEL_DIR & $ARMADEUS_TOOLS_DIR

-> build the package : sh build.sh

-> binaries are located into samba-3.0.20/source/bin directory


Tested with:
--------------

host:			FEDORA CORE 4  
cross compiler: 	gcc version 3.4.2



Installation:
-------------


1) Create the destination folder if necessary and copy the binaries into /usr/bin 
2) Create the destination folder create a file "smb.conf" into /usr/local/samba/lib
3) erase any files "/etc/mtab" & "/etc/mtab~"
4) reboot


Typical Usage : (mounting a window share)
--------------

smbmount  //192.168.1.2/SMBwindows /mnt/win 

add the options if necessary. Ex:
smbmount //192.168.1.2/SMBshare /mnt/win -o username=freddys,password=fredspass

SMBshare is the samba export name on server of the shared folder. Ex:
[SMBshare]
   path = /mycommondir
   public = yes
   writable = yes
   printable = no



Typical problems:
-----------------

mount errors : 
Make shure you are not using the mount command of the busybox! 
--> enable package "util-linux" in your buildroot configuration and install the new file system

Your kernel does not support smbfs or has no NLS.
--> rebuild and install the kernel

Lock files: 
--> erase any file "/etc/mtab~" 

Mount does not complete or Input/output errors:
/etc/mtab is not a file but a symbolic link to /proc/mounts 
--> erase /etc/mtab and reboot. Linux will create a correct /etc/mtab



See samba documentation for more details

Have fun.!



