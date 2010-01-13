#
# Main Makefile for the Armadeus drivers
#
# Add your subdirectories to obj- target
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:

obj-$(CONFIG_ARMADEUS_DRIVERS)	+= gpio/ pwm/ backlight/ isp1761/ \
	imxlkeypad/ fpga/ max1027/ max9768/ 

else

# Part executed when called from standard make in this directory:

ARMADEUS_BASE_DIR=../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(LINUX_DIR),)
LINUX_DIR:=$(ARMADEUS_LINUX_DIR)
endif
KDIR	:= $(LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

DIRS	:= $(shell ls)
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
	for directory in $(DIRS); do if [ -d "$$directory" ] && [ ! -L "$$directory" ]; then $(MAKE) -C $$directory clean; fi ; done	
	
install:
	echo "Installing Armadeus drivers..."
	 $(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
    
endif

