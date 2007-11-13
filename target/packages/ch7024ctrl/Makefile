#
# Makefile for the Armadeus SVIDEO  controler CH7024
#

CFLAGS  =       -Wall -O 

INCLUDES = 

PWD	:= $(shell pwd)

#HEADERS =  

SOURCES =       ch7024.c
OBJECTS =       ch7024.o

TARGET  = ch7024


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
	@echo "Building CH7024 control programm..."
	$(CC) $(OBJECTS) $(CFLAGS) $(INCLUDES) -o $(TARGET)


all: $(TARGET) 

clean:
	-rm -f $(TARGET) 
	-rm -f *.o *~ core

ch7024.o: ch7024.c ch7024.h  Makefile 
	@echo
	$(CC) -c $(CFLAGS) $(INCLUDES)  ch7024.c



