
ifeq ($(TARGET),)
INSTALL_DIR="../../../buildroot/build_arm_nofpu/root/usr/bin/"
CC=arm-linux-gcc
DEFINES=TARGET
CFLAGS:=`sh ../../../buildroot/build_arm_nofpu/SDL-1.2.11/sdl-config --cflags`
LIBS=`sh ../../../buildroot/build_arm_nofpu/SDL-1.2.11/sdl-config --libs`
else
INSTALL_DIR="."
CFLAGS:=`/usr/bin/sdl-config --cflags`
LIBS=`/usr/bin/sdl-config --libs`
CC=gcc
DEFINES=HOST
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

install: armanoid
	cp armanoid  $INSTALL_DIR
	cp ball.bmp  $INSTALL_DIR
	cp block.bmp $INSTALL_DIR
	cp bar.bmp   $INSTALL_DIR

clean: 
	rm -rf armanoid
	rm -rf *.o
