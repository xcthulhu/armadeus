#
# Makefile for the Armadeus bus_i2ctest drivers
#

# Part executed when cali2ctest from kernel build system:
ifneq ($(KERNELRELEASE),)

obj-m	+= i2ctest.o

i2ctest	:= i2ctest.o

# Part executed when called from standard make in this directory:
# (preferably use Makefile in parent directory)
else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR    := $(ARMADEUS_LINUX_DIR)
PWD	:= $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

test:
	$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc testi2ctest.c -o testi2ctest
	chmod 755 testi2ctest
	cp testi2ctest /tftpboot/

endif

