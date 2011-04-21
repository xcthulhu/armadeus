#
# Makefile for the Armadeus Wb_button drivers
#

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_WISHBONE_EXAMPLE_BUTTON_DRIVER)	+= board_buttons.o gbutton.o

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR := $(ARMADEUS_LINUX_DIR)
PWD  := $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

test:
	$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc testbutton.c -o testbutton

endif

