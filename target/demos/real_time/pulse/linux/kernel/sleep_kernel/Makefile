###### CONFIGURATION ######
MODULES=pulse_linux_kernel_sleep

ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULES).o
else
	ARMADEUS_BASE_DIR=../../../../../../..
	include $(ARMADEUS_BASE_DIR)/target/demos/real_time/Makefile.in
endif
