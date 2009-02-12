#
# Makefile for the Armadeus HDMI/DVI  controler AD9889
#

CFLAGS  =       -Wall -O 

INCLUDES = 

PWD	:= $(shell pwd)

#HEADERS =  

SOURCES =       ad9889ctrl.c
OBJECTS =       ad9889ctrl.o

TARGET  = ad9889ctrl


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
	@echo "Building ad9889ctrl control programm..."
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) -o $(TARGET)


all: $(TARGET) 

clean:
	-rm -f $(TARGET) 
	-rm -f *.o *~ core

ad9889.o: ad9889.c ad9889.h  Makefile 
	@echo
	$(CC) -c $(CFLAGS) $(INCLUDES)  ad9889.c



