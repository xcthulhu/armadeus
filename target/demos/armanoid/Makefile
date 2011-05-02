# Default values for local compiling:
ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
SDL_CONFIG:=$(STAGING_DIR)/usr/bin/sdl-config
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin
INSTALL_DATA_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/share
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

DEFINES=TARGET
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --cflags) -g
LIBS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_CONFIG) --libs)

ifeq ($(TARGET),x86)
	INSTALL_DIR="/usr/games/"
	INSTALL_DATA_DIR="/usr/share/"
	CFLAGS=`/usr/bin/sdl-config --cflags`
	LIBS=`/usr/bin/sdl-config --libs`
	CC=gcc
	DEFINES=HOST
endif


default: armanoid

all: armanoid

armanoid.o: armanoid.c
	$(CC) $(CFLAGS) -c -o $@ $^

collide.o: collide.c
	$(CC) $(CFLAGS) -c -o $@ $^

buzzer.o: buzzer.c
	$(CC) $(CFLAGS) -D$(DEFINES) -c -o $@ $^

armanoid: armanoid.o collide.o buzzer.o
	$(CC) $(LIBS) -o $@ $^

install: armanoid data/ball.bmp data/block.bmp data/bar.bmp
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(INSTALL_DATA_DIR)/games/armanoid/data/
	cp data/*.bmp  $(INSTALL_DATA_DIR)/games/armanoid/data/
	cp armanoid $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/armanoid

uninstall:
	rm -f $(INSTALL_DIR)/armanoid
	rm -rf $(INSTALL_DATA_DIR)/games/armanoid/data/

clean: 
	rm -rf armanoid
	rm -rf *.o

.PHONY: clean install uninstall

