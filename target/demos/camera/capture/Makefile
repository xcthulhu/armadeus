ARMADEUS_BASE_DIR=../../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

# Default values for local compiling:
STAGING_DIR:=$(ARMADEUS_STAGING_DIR)
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin/
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-sstrip
SDL_DIR:=$(ARMADEUS_SDL_DIR)
CFLAGS=$(shell STAGING_DIR=$(STAGING_DIR) sh $(SDL_DIR)/sdl-config --cflags)

#CFLAGS+=-Wall -g
LDFLAGS= -lSDL -lpthread
ifeq ($(VPU),yes)
CFLAGS+=-I$(ARMADEUS_STAGING_DIR)/usr/include/ -DUSE_VPU
LDFLAGS+= -L$(ARMADEUS_STAGING_DIR)/usr/lib/ -lvpu
endif

EXEC= capture
SRC= capture.c
ifeq ($(VPU),yes)
SRC+= rtp.c udp.c vpu_codec.c
endif
OBJ= $(SRC:.c=.o)

Q=@

all: $(EXEC)

$(EXEC): $(OBJ)
	@echo "    LD $@"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

capture.o: capture.c vpu_codec.h
	@echo "    CC $@"
	$(Q)$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.c %.h
	@echo "    CC $@"
	$(Q)$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean install

clean:
	rm -rf *.o
	rm -f $(EXEC)

install: all
	mkdir -p $(INSTALL_DIR)
	cp $(EXEC) $(INSTALL_DIR)
	$(STRIP) $(INSTALL_DIR)/$(EXEC)

