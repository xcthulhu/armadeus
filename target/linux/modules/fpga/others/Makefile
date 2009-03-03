#
# Main Makefile for the Armadeus FPGA related drivers
#
# Add your subdirectories to obj- target
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:

obj-$(CONFIG_ARMADEUS_FPGA_OTHERS_DRIVERS)	+= i2ctest/ ps2keyboard/

else

# Part executed when called from standard make in this directory:

ARMADEUS_BASE_DIR=../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(LINUX_DIR),)
LINUX_DIR:=$(ARMADEUS_LINUX_DIR)
endif
KDIR	:= $(LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../Makefile.in

install:
	echo "Installing Armadeus drivers..."
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
    
endif

