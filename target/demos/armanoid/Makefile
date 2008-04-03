
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
	INSTALL_DIR="."
	CFLAGS=`/usr/bin/sdl-config --cflags`
	LIBS=`/usr/bin/sdl-config --libs`
	CC=gcc
	DEFINES="HOST"
endif

CFLAGS+="-g"

default: armanoid

all: armanoid


armanoid.o: armanoid.c
	$(CC) $(CFLAGS) -c -o $@ $^

collide.o: collide.c
	$(CC) $(CFLAGS) -c -o $@ $^

#timer.o: timer.cpp
#	$(CC) $(CFLAGS) -c -o $@ $^

buzzer.o: buzzer.c
	$(CC) $(CFLAGS) -D$(DEFINES) -c -o $@ $^

armanoid: armanoid.o collide.o buzzer.o
	$(CC)  $(LIBS) -o $@ $^

install: armanoid ball.bmp block.bmp bar.bmp
	cp $^  $(INSTALL_DIR)

clean: 
	rm -rf armanoid
	rm -rf *.o
