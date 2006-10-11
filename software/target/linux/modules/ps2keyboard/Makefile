#
# Makefile for the Armadeus FPGA "PS/2" driver
#

BASE_DIR="../../../../buildroot/"

REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`

ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm_nofpu/linux-$(REL)

ifneq ($(KERNELRELEASE),)

obj-m	+= apf9328ps2.o
apf9328ps2-objs	:= core.o

else

KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o

install: apf9328ps2.ko
	echo "Installing drivers..."
#	cp *.ko $(BASE_DIR)build_arm_nofpu/root/lib/modules/2.6.12/kernel/drivers/
    
endif

