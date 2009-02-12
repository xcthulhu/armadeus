ARMADEUS_BASE_DIR=../../..
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(TARGET),)
	STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
        INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/bin/

	CC:=$(STAGING_DIR)/usr/bin/arm-linux-gcc
	DEFINES="-DAPF"
	CFLAGS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --cflags)
	LIBS:=$(shell STAGING_DIR=$(STAGING_DIR) sh $(ARMADEUS_SDL_DIR)/sdl-config --libs)
else
	INSTALL_DIR=./install
	CFLAGS=`/usr/bin/sdl-config --cflags`
	LIBS=`/usr/bin/sdl-config --libs`
	CC=gcc
	DEFINES="-DHOST"
endif

CFLAGS+="-g"
EXEC_NAME=input_test


default: $(EXEC_NAME)

all: $(EXEC_NAME)


main.o: main.c
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $^

$(EXEC_NAME): main.o
	$(CC)  $(LIBS) -o $@ $^

install: $(EXEC_NAME)
	mkdir -p $(INSTALL_DIR)
	cp $^  $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/input_test
clean: 
	rm -rf $(EXEC_NAME)
	rm -rf *.o

.PHONY: clean uninstall

