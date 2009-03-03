
#
# Main Makefile for the Armadeus FPGA related drivers
#
# Add your subdirectories to obj- target
#

ifneq ($(KERNELRELEASE),)

# Part executed when called from kernel build system:
obj-$(CONFIG_ARMADEUS_WISHBONE_EXAMPLE_DRIVERS)   +=  wb_button/ wb_irq_mngr/ wb_led/
	
else

default:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) zImage modules

clean:
	echo "Your Linux source dir is: $(LINUX_DIR)"
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) clean

install:
	echo "Installing Armadeus drivers..."
	$(MAKE) -C $(LINUX_DIR) SUBDIRS=$(PWD) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install

clean:
	rm -f *.*o \
	rm -f *.mod.c \
	rm -f *.cmd \
	rm -f *.o.d \
	rm -f *.order \
	rm -f .*.cmd \
	rm -rf .tmp_versions \
	rm -f Module.symvers

endif

