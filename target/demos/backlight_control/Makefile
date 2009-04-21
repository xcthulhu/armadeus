ARMADEUS_BASE_DIR=../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(TARGET),)
        ROOT_DIR:=$(ARMADEUS_ROOTFS_DIR)
        CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
        DEFINES="-DTARGET"
        CFLAGS:=$(shell STAGING_DIR=$(ARMADEUS_STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --cflags)
        LIBS:=$(shell STAGING_DIR=$(ARMADEUS_STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --libs) -lSDL_image
else
        ROOT_DIR:="/"
        CFLAGS=`/usr/bin/sdl-config --cflags`
        LIBS=`/usr/bin/sdl-config --libs` -lSDL_Image
        CC=gcc
        DEFINES="-DHOST"
endif

INSTALL_DIR:=$(ROOT_DIR)/usr/bin/
INSTALL_DIR_DATA:=$(ROOT_DIR)/usr/share/apps/backlight/data

CFLAGS+="-g"

default: backlight

all: backlight


backlight.o: backlight.c
	$(CC) $(CFLAGS) -c -o $@ $^

backlight_sysctl.o: backlight_sysctl.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $^

backlight: backlight.o backlight_sysctl.o
	$(CC)  $(LIBS) -o $@ $^

install: install_tool install_data

install_tool: backlight
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)

install_data: data/cursor.bmp data/empty.bmp data/empty_end.bmp data/full.bmp data/full_end.bmp
	mkdir -p $(INSTALL_DIR_DATA)
	cp $^  $(INSTALL_DIR_DATA)

clean: 
	rm -rf backlight
	rm -rf *.o
	rm -rf active backlight.dat

