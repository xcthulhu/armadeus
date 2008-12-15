#
# Makefile for the Armadeus Wb_button drivers
#

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_BUTTON_DRIVER)	+= button.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	-cp button.ko /tftpboot/

test:
	~/armadeus/buildroot/build_arm/staging_dir/bin/arm-linux-gcc testbutton.c -o testbutton
	chmod 755 testbutton
	cp testbutton /tftpboot/

clean:
	rm -f *.*o \
	rm -f Module.symvers

endif

