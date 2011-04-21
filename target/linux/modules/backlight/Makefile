#
# Makefile for the i.MXL backlight driver
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_IMX_BACKLIGHT) += imx_bl.o
imx_bl-objs := imx_bl_2.6.23.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

ARMADEUS_BASE_DIR=../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../Makefile.in

endif

