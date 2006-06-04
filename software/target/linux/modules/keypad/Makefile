#
# Makefile for the Armadeus keypad drivers
#
# unfinished state; to be continued

REL=2.6.12
#ifeq ($(ARMADEUS_KERNEL_DIR)),y)
ARMADEUS_KERNEL_DIR=../../../../buildroot/build_arm_nofpu/linux-$(REL)
#endif

ifneq ($(KERNELRELEASE),)

obj-m   += imxkeypad.o

else
#KDIR	:= /lib/modules/$(shell uname -r)/build
KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o

install:
	echo "Installing drivers..."
	echo "type: cp *.ko ../../../../../software/buildroot/build_arm_nofpu/root/lib/modules/2.6.12/kernel/drivers/"
    
endif

