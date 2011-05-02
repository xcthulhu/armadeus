ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

# Default values for local compiling:
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin
INSTALL_DATA_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/share
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
DEFINES=-DTARGET
STAGING_DIR:=$(ARMADEUS_STAGING_DIR)
SDL_CONFIG:=$(STAGING_DIR)/usr/bin/sdl-config
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --cflags) -g
LDFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --libs) -lSDL_image

ifeq ($(TARGET),x86)
        INSTALL_DIR:=/usr/local/bin
        INSTALL_DATA_DIR:=/usr/share
        CFLAGS=`/usr/bin/sdl-config --cflags`
        LDFLAGS=`/usr/bin/sdl-config --libs` -lSDL_Image
        CC=gcc
        STRIP=strip
        DEFINES=-DHOST
endif


default: backlight

all: backlight

backlight.o: backlight.c
	$(CC) $(CFLAGS) -c -o $@ $^

backlight_sysctl.o: backlight_sysctl.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $^

backlight: backlight.o backlight_sysctl.o
	$(CC) $(LDFLAGS) -o $@ $^

install: install_tool install_data

install_tool: backlight
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/$^

install_data: data/cursor.bmp data/empty.bmp data/empty_end.bmp data/full.bmp data/full_end.bmp
	mkdir -p $(INSTALL_DATA_DIR)/apps/backlight/data
	cp $^  $(INSTALL_DATA_DIR)/apps/backlight/data/

uninstall:
	rm -f $(INSTALL_DIR)/backlight

clean:
	rm -rf backlight
	rm -rf *.o
	rm -rf active backlight.dat

.PHONY: clean install uninstall

