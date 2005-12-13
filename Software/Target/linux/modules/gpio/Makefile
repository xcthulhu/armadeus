#
# Makefile for the Armadeus GPIO drivers
#

REL=2.6.10-imx1

ifneq ($(KERNELRELEASE),)

obj-m	+= gpio.o
obj-m	+= loadfpga.o
obj-m   += ppdev.o

gpio-objs	:= core.o
#loadfpga-objs   := fpga.o
ppdev-objs  := ppdevemu.o

else
#KDIR	:= /lib/modules/$(shell uname -r)/build
KDIR	:= /lib/modules/$(REL)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o
    
endif

