
ifeq ($(TARGET),)
	BUILDROOT_DIR:=../../../buildroot/
	BUILD_DIR_EXT:=$(shell ls $(BUILDROOT_DIR) | grep -e "^build_arm*")
	BUILD_DIR:=$(BUILDROOT_DIR)/$(BUILD_DIR_EXT)

	SDL_DIR:=$(BUILD_DIR)/SDL-1.2.11/
	STAGING_DIR:=$(BUILD_DIR)/staging_dir/
	ROOT_DIR:=$(BUILDROOT_DIR)/project_$(BUILD_DIR_EXT)/armadeus/root/
        INSTALL_DIR:=$(ROOT_DIR)/usr/bin/

	CC:=$(STAGING_DIR)/bin/arm-linux-gcc
	DEFINES="TARGET"
	CFLAGS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --cflags)
	LIBS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --libs)
else
	INSTALL_DIR="./install"
	CFLAGS=`/usr/bin/sdl-config --cflags`
	LIBS=`/usr/bin/sdl-config --libs`
	CC=gcc
	DEFINES="HOST"
endif

CFLAGS+="-g"

default: backlight

all: backlight


backlight.o: backlight.c
	$(CC) $(CFLAGS) -c -o $@ $^

backlight_sysctl.o: backlight_sysctl.c
	$(CC) $(CFLAGS) -D$(DEFINES) -c -o $@ $^

backlight: backlight.o backlight_sysctl.o
	$(CC)  $(LIBS) -o $@ $^

install: backlight cursor.bmp empty.bmp empty_end.bmp full.bmp full_end.bmp
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)

clean: 
	rm -rf backlight
	rm -rf *.o
	rm -rf active backlight.dat

