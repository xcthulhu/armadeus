#
# Makefile for the i.MXL backlight driver
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_IMX_BACKLIGHT) += imx_bl.o

ifeq ($(KERNELVERSION),2.6.18.1)
	imx_bl-objs := imx_bl_2.6.1x.o
else
	imx_bl-objs := imx_bl_2.6.23.o
endif

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

ARMADEUS_BASE_DIR=../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o
	rm -f Module.symvers
	rm -f .*.cmd

endif

