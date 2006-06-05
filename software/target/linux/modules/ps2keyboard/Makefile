#
# Makefile for the Armadeus FPGA "PS/2" driver
#

ifeq ($(ARMADEUS_KERNEL_DIR)),y)
ARMADEUS_KERNEL_DIR="/Please define ARMADEUS_KERNEL_DIR envt variable before trying to compile this driver !"
endif

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
#	cp *.ko ../../../../../software/buildroot/build_arm_nofpu/root/lib/modules/2.6.12/kernel/drivers/
    
endif

