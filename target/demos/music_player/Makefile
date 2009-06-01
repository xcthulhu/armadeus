# Compil ARM 
ifeq ($(TARGET),arm)
ARMADEUS_BASE_DIR:=$(shell pwd)/../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in
STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/bin/
CC=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
DEFINES="-DAPF"
CFLAGS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --cflags) -g
LIBS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --libs) -lSDL_ttf -lSDL_mixer

else
# Compil HOST
CC=gcc
CFLAGS=`/usr/bin/sdl-config --cflags`
LIBS=`/usr/bin/sdl-config --libs` -lSDL_ttf -lSDL_mixer
DEFINES="-DHOST"

endif


EXEC_NAME=aplayer
default: $(EXEC_NAME)

all: $(EXEC_NAME)


main.o: player.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $^

$(EXEC_NAME): player.o
	$(CC)  $(LIBS) -o $@ $^

clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

.PHONY: clean uninstall

