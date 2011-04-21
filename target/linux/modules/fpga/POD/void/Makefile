#
# Makefile for a generic Armadeus drivers
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_POD_DRIVERS)	+= void.o

else

# Part executed when called from standard make in this directory:

ARMADEUS_BASE_DIR=../../../../../../
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR := $(ARMADEUS_LINUX_DIR)
PWD  := $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

endif

