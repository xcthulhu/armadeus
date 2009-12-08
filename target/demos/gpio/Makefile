# Default values for local compiling:
ARMADEUS_BASE_DIR:=$(shell pwd)/../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin/
CC=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip
DEFINES=-D$(ARMADEUS_BOARD_NAME)


EXEC_NAME = blink_led

default: $(EXEC_NAME)

all: $(EXEC_NAME)

$(EXEC_NAME): blink_led.c
	$(CC) $(CFLAGS) $(DEFINES) -Wall -o $@ $^

clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

install: $(EXEC_NAME)
	cp $(EXEC_NAME) $(INSTALL_DIR)/
	$(STRIP) $(INSTALL_DIR)/$(EXEC_NAME)

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC_NAME)

.PHONY: clean install uninstall
