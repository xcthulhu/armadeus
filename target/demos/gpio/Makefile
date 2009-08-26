
ifneq ($(CC),)
# Locally compiled:
ARMADEUS_BASE_DIR:=$(shell pwd)/../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in
STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/bin/
CC=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
DEFINES=-D$(ARMADEUS_BOARD_NAME)
endif

EXEC_NAME = blink_led

default: $(EXEC_NAME)

all: $(EXEC_NAME)

$(EXEC_NAME): blink_led.c
	$(CC) $(CFLAGS) $(DEFINES) -Wall -o $@ $^

clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

install:
	cp $(EXEC_NAME) $(INSTALL_DIR)/

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC_NAME)

.PHONY: clean install uninstall
