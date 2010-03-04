# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../../
include $(ARMADEUS_BASE_DIR)/Makefile.in

CFLAGS=-Wall -g -O0
LDFLAGS=
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
CXX:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-g++
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

C_BINARY=test_cdevices
C_LIBNAME=as_devices
C_LIB=c/lib${C_LIBNAME}.so.1
C_INCLUDE=c/

CPP_BINARY=test_cppdevices
CPP_LIBNAME=as_devices_cpp
CPP_LIB=cpp/lib${CPP_LIBNAME}.so.1
CPP_INCLUDE=cpp/

.PHONY: all

all: ${C_BINARY} 
	#${CPP_BINARY}

${C_BINARY}: main.c test_c.h ${C_LIB}
	${CC} ${CFLAGS} main.c ${C_LIB} -o ${C_BINARY} -I${C_INCLUDE} -lm

#${CPP_BINARY}: main.c test_cpp.h ${CPP_LIB} ${C_LIB}
#	${CXX} ${CFLAGS} main.c ${C_LIB} ${CPP_LIB} -I${C_INCLUDE} -I${CPP_INCLUDE} -o ${CPP_BINARY} 
#
#${CPP_LIB}: ${C_LIB}
#	cd cpp; make CXX=${CXX}

${C_LIB}:
	cd c; make CC=${CC} 

clean:
	cd c; make clean ; cd ../cpp; make clean; cd ../python; make clean
	-rm ${C_BINARY} ${CPP_BINARY}

