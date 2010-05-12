ARMADEUS_BASE_DIR=../../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

# Default values for local compiling:
STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin/
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
SDL_DIR:=$(ARMADEUS_SDL_DIR)
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --cflags) -g

LDFLAGS= -lSDL -lpthread

EXEC=capture
SRC= $(wildcard *.c *.h)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) 

.PHONY: clean install

clean:
	rm -rf *.o
	rm -f $(EXEC)

install: all
	mkdir -p $(INSTALL_DIR)
	cp $(EXEC) $(INSTALL_DIR)

