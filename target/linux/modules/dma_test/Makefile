#
# Makefile for the DMA throughput test driver
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-m	+= dma.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

ARMADEUS_BASE_DIR=../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets:
include ../Makefile.in

endif

