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
INSTALL_DIR:=$(ARMADEUS_STAGING_DIR)

C_BINARY=test_cdevices
C_LIBNAME=libas_devices.so
C_LIB=c/$(C_LIBNAME)
C_INCLUDE=c

P_WRAPPER=.python_wrappers

CPP_BINARY=test_cppdevices
CPP_LIBNAME=libas_devices_cpp.so
CPP_LIB=cpp/$(CPP_LIBNAME)
CPP_INCLUDE=cpp


.PHONY: all

all: $(C_BINARY) $(P_WRAPPER)
	#$(CPP_BINARY)

$(C_BINARY): main.c test_c.h $(C_LIB)
	$(CC) $(CFLAGS) main.c $(C_LIB) -o $(C_BINARY) -I$(C_INCLUDE) -lm

#$(CPP_BINARY): main.c test_cpp.h $(CPP_LIB) $(C_LIB)
#	$(CXX) $(CFLAGS) main.c $(C_LIB) $(CPP_LIB) -I$(C_INCLUDE) -I$(CPP_INCLUDE) -o $(CPP_BINARY)
#
#$(CPP_LIB): $(C_LIB)
#	cd cpp; make CXX=$(CXX)

$(C_LIB):
	make -C c/

$(P_WRAPPER):
	make -C python/
	touch $(P_WRAPPER)

install:
	install -D $(C_LIB) $(INSTALL_DIR)/usr/lib/$(C_LIBNAME)
	mkdir -p $(INSTALL_DIR)/usr/include/as_devices/
	cp $(C_INCLUDE)/*.h $(INSTALL_DIR)/usr/include/as_devices/

install-exe:
	install -D $(C_LIB) $(INSTALL_DIR)/usr/lib/$(C_LIBNAME)
	$(STRIP) $(INSTALL_DIR)/usr/lib/$(C_LIBNAME)

clean:
	make -C c/ clean
	rm -f $(P_WRAPPER)
	make -C python/ clean
	rm -f $(C_BINARY) # $(CPP_BINARY)

