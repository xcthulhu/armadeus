# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(ARMADEUS_BOARD_NAME),apf9328)
CFLAGS=-Wall -g -O0 -DAPF9328
else
ifeq ($(ARMADEUS_BOARD_NAME),apf27)
CFLAGS=-Wall -g -O0 -DAPF27
endif
endif

LDFLAGS=
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
CXX:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-g++
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

C_BINARY=test_cdevices
C_LIBNAME=as_devices
C_LIB=c/lib${C_LIBNAME}.so.1
C_INCLUDE=c/

P_WRAPPER=.python_wrappers

CPP_BINARY=test_cppdevices
CPP_LIBNAME=as_devices_cpp
CPP_LIB=cpp/lib${CPP_LIBNAME}.so.1
CPP_INCLUDE=cpp/

.PHONY: all

all: ${C_BINARY} ${P_WRAPPER}
	#${CPP_BINARY}

${C_BINARY}: main.c test_c.h ${C_LIB}
	${CC} ${CFLAGS} main.c ${C_LIB} -o ${C_BINARY} -I${C_INCLUDE} -lm

#${CPP_BINARY}: main.c test_cpp.h ${CPP_LIB} ${C_LIB}
#	${CXX} ${CFLAGS} main.c ${C_LIB} ${CPP_LIB} -I${C_INCLUDE} -I${CPP_INCLUDE} -o ${CPP_BINARY}
#
#${CPP_LIB}: ${C_LIB}
#	cd cpp; make CXX=${CXX}

${C_LIB}:
	cd c; make

${P_WRAPPER}:
	cd python; make
	touch ${P_WRAPPER}

clean:
	cd c; make clean ;
	-rm ${P_WRAPPER}
	cd python; make clean
	-rm ${C_BINARY} # ${CPP_BINARY}

