
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

default: keypad_test

all: keypad_test


keypad.o: keypad.c
	$(CC) $(CFLAGS) -c -o $@ $^

keypad_test: keypad.o
	$(CC)  $(LIBS) -o $@ $^

install: keypad_test
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)

clean: 
	rm -rf keypad_test
	rm -rf *.o

