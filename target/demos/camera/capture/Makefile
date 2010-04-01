CFLAGS=-Wall
LDFLAGS= -lSDL
ifeq ($(PREFIX),)
# Host
CC=gcc
CFLAGS+= -O2
else
# APF
CC=${PREFIX}/usr/bin/arm-linux-uclibc-gcc
CFLAGS+= -Os
INSTALL_DIR=/tftpboot
endif

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
	cp -f $(EXEC) $(INSTALL_DIR)

