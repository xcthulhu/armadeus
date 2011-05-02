ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

# Default values for local compiling:
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin
INSTALL_DATA_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/share
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
STAGING_DIR:=$(ARMADEUS_STAGING_DIR)
SDL_CONFIG:=$(STAGING_DIR)/usr/bin/sdl-config
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --cflags) -g
LDFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --libs)

ifeq ($(TARGET),x86)
        INSTALL_DIR:=/usr/local/bin
        INSTALL_DATA_DIR:=/usr/share
        CFLAGS=`/usr/bin/sdl-config --cflags`
        LDFLAGS=`/usr/bin/sdl-config --libs`
        CC=gcc
        STRIP=strip
endif


EXEC_NAME=test_lcd

default: $(EXEC_NAME)

all: $(EXEC_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(EXEC_NAME): test_lcd.o
	$(CC) $(LDFLAGS) -o $@ $^

install: $(EXEC_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/$^

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC_NAME)

clean:
	rm -rf $(EXEC_NAME)
	rm -rf *.o

.PHONY: clean install uninstall

