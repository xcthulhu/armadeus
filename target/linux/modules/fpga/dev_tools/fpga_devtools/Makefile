#
# Makefile for the Armadeus FPGA loader driver
# PATH=$HOME/nico/test/buildroot/build_arm_nofpu/staging_dir/bin:$PATH
# make ARCH=arm
# tftp -gr fpgaloader.ko -l /lib/modules/drivers/fpgaloader.ko 192.168.0.3
# tftp -gr fpga_test.bit 192.168.0.3
# insmod /lib/modules/drivers/fpgaloader.ko
# dd if=fpga_test.bit of=/dev/fpga/loader


BASE_DIR="../../../../buildroot/"

REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`

ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm_nofpu/linux-$(REL)

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_FPGA_TOOL_DRIVER)	+= fpgaloader.o
fpgaloader-objs   := target-fpga-loader.o xilinx-fpga-loader.o fpga-loader.o 

else
KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o \
    rm -f Module.symvers

   
endif

