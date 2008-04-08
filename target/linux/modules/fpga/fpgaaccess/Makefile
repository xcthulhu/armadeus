#
# Makefile for the Armadeus Wb_led drivers
#

BASE_DIR="../../../../../buildroot/"

REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`

ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm/linux-$(REL)

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_FPGAACCESS_DRIVER)	+= fpgaaccess.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)

else
KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	-cp fpgaaccess.ko /tftpboot/

fpgaregs: fpgaregs.c
	~/armadeus/buildroot/build_arm/staging_dir/bin/arm-linux-gcc -Wall fpgaregs.c -o fpgaregs
	chmod 755 fpgaregs
	cp fpgaregs /tftpboot/

clean:
	rm -f *.*o \
	rm -f Module.symvers

endif

