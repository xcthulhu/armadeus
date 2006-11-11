#
# Makefile for the Armadeus DAC  control
#

CFLAGS  =       -Wall -O 

INCLUDES = 

PWD	:= $(shell pwd)

#HEADERS =  

SOURCES =       setDAC.c
OBJECTS =       setDAC.o

TARGET  = setDAC


####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

.cxx.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

.cc.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

.C.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<


####### Build rules


$(TARGET): $(OBJECTS) Makefile $(HEADERS)
	@echo
	@echo "Building I2C dac test programm..."
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) -o $(TARGET)


all: $(TARGET) 

clean:
	-rm -f $(TARGET) 
	-rm -f *.o *~ core

setDAC.o: setDAC.c max5821.h  Makefile 
	@echo
	$(CC) -c $(CFLAGS) $(INCLUDES)  setDAC.c



