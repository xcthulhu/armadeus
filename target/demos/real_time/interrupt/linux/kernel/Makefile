###### CONFIGURATION ######
MODULES=interrupt_linux_kernel
ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULES).o
	EXTRA_CFLAGS=-I./include/xenomai -I./include/xenomai/posix -I./include/xenomai/native
else
ARMADEUS_BASE_DIR=../../../../../..
	include $(ARMADEUS_BASE_DIR)/target/demos/real_time/Makefile.in
endif
