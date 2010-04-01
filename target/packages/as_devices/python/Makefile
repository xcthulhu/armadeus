# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../../
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

INCLUDE_LIB=../c/

AS_LIB=../c/libas_devices.so

LIBNAME=AsDevices

WRAPPER_LIBS=AsGpio_wrap.so
WRAPPER_DIR=src

PYTHON_VERS=2.4
PYTHON_SUBVERS=5
PYINC=/usr/include/python2.4
PYLIB=../../../../buildroot/project_build_armv5te/apf27/root/usr/lib/python${PYTHON_VERS}

.PHONY: all

all: ${WRAPPER_LIBS}

%.so: %.o
	${CC} -shared $< -o $@ -lc -L${PYLIB} ${AS_LIB}
	mv $@ ${LIBNAME}/wrappers
	rm $<

%.o : ${WRAPPER_DIR}/%.c
	${CC} -fpic -o $@ -c $< ${CFLAGS} -L${PYLIB} -I${PYINC} -I${INCLUDE_LIB}

clean:
	-rm ${LIBNAME}/wrappers/*.so
