#
# Makefile for the Armadeus FPGA led driver
#

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_ARMADEUS_WISHBONE_EXAMPLE_LED_DRIVER)	+= board_leds.o g_led.o

else

ARMADEUS_BASE_DIR=../../../../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

KDIR := $(ARMADEUS_LINUX_DIR)
PWD  := $(shell pwd)

# Armadeus custom drivers common targets (default, clean, etc...)
include ../../../Makefile.in

test:
	$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc -Wall testled.c -o testled
	chmod 755 testled

endif

