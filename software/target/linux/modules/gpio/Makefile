#
# Makefile for the Armadeus GPIO drivers
#

BASE_DIR="../../../../buildroot/"

REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`

ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm_nofpu/linux-$(REL)


ifneq ($(KERNELRELEASE),)

obj-m	+= gpio.o
obj-m	+= loadfpga.o
obj-m   += ppdev.o

gpio-objs	:= core.o
#loadfpga-objs   := fpga.o
ppdev-objs  := ppdevemu.o

else
#KDIR	:= /lib/modules/$(shell uname -r)/build
#KDIR	:= /lib/modules/$(REL)/build
KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o

install: gpio.ko
	echo "Installing drivers..."
#	cp *.ko $(BASE_DIR)build_arm_nofpu/root/lib/modules/2.6.12/kernel/drivers/
    
endif

