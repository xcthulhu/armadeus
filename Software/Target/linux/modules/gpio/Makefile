#
# Makefile for the Armadeus GPIO drivers
#

REL=2.6.10-imx1
ifeq ($(ARMADEUS_KERNEL_DIR)),y)
ARMADEUS_KERNEL_DIR="/Please define ARMADEUS_KERNEL_DIR before trying to compile this driver !"
endif

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
    
endif

