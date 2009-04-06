### List of modules to be build
MODULES= interrupt_xeno_kernel
XENOMAI=y

ifneq ($(KERNELRELEASE),)
	EXTRA_CFLAGS := -I./include/xenomai -I./include/xenomai/posix -I./include/xenomai/native
	obj-m := $(MODULES).o
else
	ARMADEUS_BASE_DIR=../../../../../..
	include $(ARMADEUS_BASE_DIR)/target/demos/real_time/Makefile.in
endif   
