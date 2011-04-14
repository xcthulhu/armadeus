# Default values for local compiling:
ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin
INIT_DIR:=$(ARMADEUS_ROOTFS_DIR)/etc/init.d
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

DEFINES=TARGET
CFLAGS= -Wall

default: mcp25020_ctrl

all: mcp25020_ctrl

mcp25020_ctrl.o: mcp25020_ctrl.c
	$(CC) $(CFLAGS) -c -o $@ $^

install: mcp25020_ctrl
	mkdir -p $(INSTALL_DIR)
	cp $^ $(INSTALL_DIR)
	cp init_can.sh $(INIT_DIR)/S50init_can.sh
	$(STRIP) $(INSTALL_DIR)/mcp25020_ctrl

uninstall:
	rm -f $(INSTALL_DIR)/mcp25020_ctrl
	rm -f $(INIT_DIR)/S50init_can.sh

clean: 
	rm -rf mcp25020_ctrl
	rm -rf *.o

.PHONY: clean install uninstall

