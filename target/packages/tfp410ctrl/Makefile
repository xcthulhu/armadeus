#
# Makefile for the Armadeus HDMI/DVI  controler TFP410
#

CFLAGS  =       -Wall -O 

INCLUDES = 

PWD	:= $(shell pwd)

#HEADERS =  

SOURCES =       tfp410ctrl.c
OBJECTS =       tfp410ctrl.o

TARGET  = tfp410ctrl


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
	@echo "Building tfp410ctrl control programm..."
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) -o $(TARGET)


all: $(TARGET) 

clean:
	-rm -f $(TARGET) 
	-rm -f *.o *~ core

tfp410ctrl.o: tfp410ctrl.c tfp410ctrl.h  Makefile 
	@echo
	$(CC) -c $(CFLAGS) $(INCLUDES)  tfp410ctrl.c



