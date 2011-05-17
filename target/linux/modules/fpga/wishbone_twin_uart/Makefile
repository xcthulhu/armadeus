#
# Makefile for the Armadeus FPGA serial "driver" (compatible with 16750/8250 OpenCore IP)
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:

obj-$(CONFIG_SERIAL_8250)	+= 16750_ocore.o
16750_ocore-objs	:= 16750.o

else

ARMADEUS_BASE_DIR=../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR := $(ARMADEUS_LINUX_DIR)
PWD  := $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../Makefile.in

endif

