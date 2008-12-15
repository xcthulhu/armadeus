#
# Makefile for the Armadeus FPGA loader driver
# PATH=$HOME/nico/test/buildroot/build_arm_nofpu/staging_dir/bin:$PATH
# make ARCH=arm
# tftp -gr fpgaloader.ko -l /lib/modules/drivers/fpgaloader.ko 192.168.0.3
# tftp -gr fpga_test.bit 192.168.0.3
# insmod /lib/modules/drivers/fpgaloader.ko
# dd if=fpga_test.bit of=/dev/fpga/loader


ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_FPGA_TOOL_DRIVER)	+= fpgaloader.o
fpgaloader-objs   := target-fpga-loader.o xilinx-fpga-loader.o fpga-loader.o 

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o \
    rm -f Module.symvers
   
endif

