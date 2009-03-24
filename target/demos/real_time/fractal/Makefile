CC=arm-linux-gcc
CFLAGS=-g -W -Wall
LDFLAGS=
EXEC=newton
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

newton: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean install

clean:
	rm -rf *.o
	rm -f $(EXEC)
	rm -f *.c~ *.h~ Makefile~
install: all
	cp -f $(EXEC) /tftpboot/local/bin

