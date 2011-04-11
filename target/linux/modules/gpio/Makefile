#
# Makefile for the Armadeus GPIO drivers
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_GPIO_DRIVER)	+= gpio.o
obj-$(CONFIG_ARMADEUS_PPDEV_DRIVER)	+= ppdev.o

gpio-objs	:= core.o
ppdev-objs	:= ppdevemu.o

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

