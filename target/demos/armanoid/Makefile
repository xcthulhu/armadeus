
ifeq ($(TARGET),arm)
INCLUDE_PATH="../../../buildroot/build_arm_nofpu/SDL-1.2.9/include/"
LIBRARY_PATH="../../../buildroot/build_arm_nofpu/root/usr/lib/"
INSTALL_DIR="../../../buildroot/build_arm_nofpu/root/usr/bin/"
CC=arm-linux-gcc
DEFINES=TARGET
else
INCLUDE_PATH="/usr/include/SDL"
LIBRARY_PATH="/usr/lib/"
INSTALL_DIR="."
CC=gcc
DEFINES=HOST
endif

CFLAGS="-g"

default: armanoid

all: armanoid


armanoid.o: armanoid.c
	$(CC) -I$(INCLUDE_PATH) $(CFLAGS) -c -o $@ $^

collide.o: collide.c
	$(CC) -I$(INCLUDE_PATH) $(CFLAGS) -c -o $@ $^

#timer.o: timer.cpp
#	$(CC) -I$(INCLUDE_PATH) $(CFLAGS) -c -o $@ $^

buzzer.o: buzzer.c
	$(CC) -I$(INCLUDE_PATH) $(CFLAGS) -D$(DEFINES) -c -o $@ $^

armanoid: armanoid.o collide.o buzzer.o
	$(CC) -I$(INCLUDE_PATH) -L$(LIBRARY_PATH) -o $@ $^ -lSDL

install: armanoid
	cp armanoid  $INSTALL_DIR
	cp ball.bmp  $INSTALL_DIR
	cp block.bmp $INSTALL_DIR
	cp bar.bmp   $INSTALL_DIR

clean: 
	rm -rf armanoid
	rm -rf *.o
