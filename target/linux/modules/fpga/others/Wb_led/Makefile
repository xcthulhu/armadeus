#
# Makefile for the Armadeus Wb_led drivers
#

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_LED_DRIVER)	+= s_led.o g_led.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	-cp g_led.ko /tftpboot/
	-cp s_led.ko /tftpboot/

test:
	~/armadeus/buildroot/build_arm/staging_dir/bin/arm-linux-gcc -Wall testled.c -o testled
	chmod 755 testled
	cp testled /tftpboot/

clean:
	rm -f *.*o \
	rm -f Module.symvers
	rm -rf .*.cmd
	rm -f *.mod.c
    
endif

