ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

# Default values for local compiling:
STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin/
INSTALL_DATA_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/share
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
SDL_DIR:=$(ARMADEUS_SDL_DIR)
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --cflags) -g
LIBS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --libs)
DEFINES=-DAPF

ifeq ($(TARGET),x86)
	INSTALL_DIR=./install
	INSTALL_DATA_DIR=./install/images
	CFLAGS=`/usr/bin/sdl-config --cflags`
	LIBS=`/usr/bin/sdl-config --libs`
	CC=gcc
	STRIP=strip
	DEFINES=-DHOST
endif


EXEC_NAME=input_test

default: $(EXEC_NAME)

all: $(EXEC_NAME)

main.o: main.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $^

$(EXEC_NAME): main.o
	$(CC)  $(LIBS) -o $@ $^

install: $(EXEC_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $(EXEC_NAME) $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/$(EXEC_NAME)
	mkdir -p $(INSTALL_DATA_DIR)/images
	cp sdl.bmp $(INSTALL_DATA_DIR)/images/

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC_NAME)

clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

.PHONY: clean install uninstall

