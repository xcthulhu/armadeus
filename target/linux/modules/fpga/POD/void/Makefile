#
# Makefile for a generic Armadeus drivers
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_VOID_DRIVER)	+= void.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

BASE_DIR="../../../../../../buildroot/"
REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BASE_DIR).config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`
ARMADEUS_KERNEL_DIR=$(BASE_DIR)build_arm/linux-$(REL)
KDIR    := $(ARMADEUS_KERNEL_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.*o
	rm -f Module.symvers

endif

