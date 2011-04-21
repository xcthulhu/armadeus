
#
# Main Makefile for the Armadeus FPGA related drivers
#
# Add your subdirectories to obj- target
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:
obj-$(CONFIG_ARMADEUS_WISHBONE_EXAMPLE_DRIVERS)   +=  wb_button/ wb_irq_mngr/ wb_led/
	
else

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

install:
	echo "Installing Armadeus drivers..."
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install

endif

