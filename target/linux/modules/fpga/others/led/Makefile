#
# Makefile for the Armadeus bus_led drivers
#

# Part executed when called from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_GPIO_DRIVER)	+= led.o

led	:= led.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	cp led.ko /tftpboot/

test:
	~/armadeus/buildroot/build_arm/staging_dir/bin/arm-linux-gcc testled.c -o testled
	chmod 755 testled
	cp testled /tftpboot/

clean:
	rm -f *.*o \
	rm -f Module.symvers

endif

