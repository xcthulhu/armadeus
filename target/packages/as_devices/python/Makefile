# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../../
-include $(ARMADEUS_BASE_DIR)/Makefile.in

CFLAGS=-Wall -g -O0

LDFLAGS=
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
CXX:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-g++
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

INCLUDE_LIB=../c/

AS_LIB=../c/libas_devices.so

LIBNAME=AsDevices

WRAPPER_LIBS=AsGpio_wrap.so
WRAPPER_LIBS+= AsAdc_wrap.so
WRAPPER_LIBS+= AsDac_wrap.so
WRAPPER_LIBS+= AsI2c_wrap.so
WRAPPER_DIR=src

PYTHON_VERS=2.4
PYTHON_SUBVERS=5
STAGING_DIR=$(ARMADEUS_STAGING_DIR)
PYINC=$(STAGING_DIR)/usr/include/python2.4/
PYLIB=$(ARMADEUS_TARGET_DIR)/usr/lib/python$(PYTHON_VERS)

.PHONY: all

all: $(WRAPPER_LIBS)

%.so: %.o
	$(CC) -shared $< -o $@ -L$(PYLIB) $(AS_LIB)
	mv $@ $(LIBNAME)/wrappers
	rm $<

%.o : $(WRAPPER_DIR)/%.c
	$(CC) -fpic -o $@ -c $< $(CFLAGS) -L$(PYLIB) -I$(PYINC) -I$(INCLUDE_LIB)

clean:
	rm -f $(LIBNAME)/wrappers/*.so

