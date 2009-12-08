ARMADEUS_BASE_DIR=../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
STAGING_DIR:=$(ARMADEUS_STAGING_DIR)
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin/
SDL_DIR:=$(ARMADEUS_SDL_DIR)

DEFINES=-DTARGET
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --cflags) -g
LDFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --libs) -lSDL_image

ifeq ($(TARGET),x86)
	INSTALL_DIR:=/usr/local/bin/
	CFLAGS=`/usr/bin/sdl-config --cflags` -g
	LIBS=`/usr/bin/sdl-config --libs` -lSDL_Image
	CC=gcc
	STRIP=strip
	DEFINES=-DHOST
endif


EXEC_NAME=show_image

default: $(EXEC_NAME)

all: $(EXEC_NAME)

image.o: image.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(EXEC_NAME): image.o
	$(CC) $(LDFLAGS) -o $@ $^

install: $(EXEC_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $^ $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/$^

clean:
	rm -rf $(EXEC_NAME)
	rm -rf *.o

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC_NAME)

.PHONY: clean install uninstall

