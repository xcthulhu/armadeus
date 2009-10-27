#
# Makefile for the Armadeus DAC control tool
#

CFLAGS  = -Wall -O 

INCLUDES = 

PWD	:= $(shell pwd)

SOURCES = setDAC.c
OBJECTS = setDAC.o
TARGET  = setDAC


$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) -o $(TARGET)

all: $(TARGET) 

clean:
	-rm -f $(TARGET) 
	-rm -f *.o *~ core

setDAC.o: setDAC.c max5821.h
	$(CC) -c $(CFLAGS) $(INCLUDES) setDAC.c

