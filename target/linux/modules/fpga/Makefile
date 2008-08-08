#
# Main Makefile for the Armadeus FPGA related drivers
#
# Add your subdirectories to obj- target
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:

obj-$(CONFIG_ARMADEUS_FPGA_DRIVERS)	+= dev_tools/ others/ POD/

else

# Part executed when called from standard make in this directory:

BASE_DIR:=../../../../buildroot/
REL:=$(shell grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed -e 's/BR2_PACKAGE_LINUX_VERSION=//' | sed -e 's/"//g')
#`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
#      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`
ARMADEUS_KERNEL_DIR:=$(BASE_DIR)build_arm/linux-$(REL)
ifeq ($(LINUX_DIR),)
LINUX_DIR:=$(ARMADEUS_KERNEL_DIR)
endif
PWD:= $(shell pwd)

default:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) zImage modules

clean:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) clean

install:
	echo "Installing Armadeus drivers..."
	 $(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
    
endif

