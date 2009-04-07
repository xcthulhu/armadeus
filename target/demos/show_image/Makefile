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
ifneq ($(DEBUG),)
CFLAGS+="-g"
endif


EXEC_NAME=show_image

default: $(EXEC_NAME)

all: $(EXEC_NAME)


image.o: image.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(EXEC_NAME): image.o
	$(CC)  $(LIBS) -o $@ $^


install: $(EXEC_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)

clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

.PHONY: clean install

