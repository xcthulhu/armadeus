#
# Makefile for the ISP1761
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_ISP1761)	+= hal/ host/ apf9328-isp1761.o

# Part execute when called from standard make in this directory:
else

ARMADEUS_BASE_DIR=../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(LINUX_DIR),)
LINUX_DIR    := $(ARMADEUS_LINUX_DIR)
endif
KDIR	:= $(LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../Makefile.in

endif
