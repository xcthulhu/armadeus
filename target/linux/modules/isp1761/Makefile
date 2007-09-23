#
# Makefile for the ISP1761
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_ISP1761)	+= hal/ host/ apf9328-isp1761.o

# Part execute when called from standard make in this directory:
else

BASE_DIR="../../../../buildroot/"
REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`
ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm_nofpu/linux-$(REL)
ifeq ($(LINUX_DIR),)
LINUX_DIR    := $(ARMADEUS_KERNEL_DIR)
endif
PWD	:= $(shell pwd)

default:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) modules

clean:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) clean

install:
	echo "Installing Armadeus drivers..."
	 $(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
    
endif
