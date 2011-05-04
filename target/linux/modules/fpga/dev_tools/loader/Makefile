#
# Makefile for the Armadeus FPGA loader driver
#

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_FPGA_LOADER_DRIVER)	+= fpgaloader.o
fpgaloader-objs := xilinx-fpga-loader.o fpga-loader.o
ifneq ($(CONFIG_MACH_APF51),)
obj-y += apf51-fpga-loader.o
endif
ifneq ($(CONFIG_MACH_APF27),)
obj-y += apf27-fpga-loader.o
endif
ifneq ($(CONFIG_MACH_APF9328),)
obj-y += apf9328-fpga-loader.o
endif

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

endif

