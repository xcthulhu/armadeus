# Default values for local compiling:
#
ARMADEUS_BASE_DIR=../../../../
include $(ARMADEUS_BASE_DIR)/Makefile.in

ifeq ($(ARMADEUS_BOARD_NAME),apf9328)
CFLAGS=-Wall -g -O0 -DAPF9328
endif

ifeq ($(ARMADEUS_BOARD_NAME),apf27)
CFLAGS=-Wall -g -O0 -DAPF27
endif

LDFLAGS=
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

OBJ=as_pwm.o
OBJ+= as_i2c.o
OBJ+= as_gpio.o
OBJ+= as_93lcxx.o
OBJ+= as_spi.o
OBJ+= as_max1027.o

LIBRARY=as_devices
LIB_VERS=1
STAT_LIB=${LIBRARY}.a
SONAME=lib${LIBRARY}.so
LIBNAME=${SONAME}.${LIB_VERS}
ARCHIVE_CMD=ar
ARCHIVE_OPTS=-cvq

${LIBNAME}: ${OBJ}
	${CC} -shared -Wl,-soname,${SONAME} -o ${SONAME} ${OBJ} -lc 
	ln -sf ${SONAME} ${LIBNAME}

${STAT_LIB}: ${OBJ} 
	${ARCHIVE_CMD} ${ARCHIVE_OPTS} ${STAT_LIB} ${OBJ}

%.o: %.c
	$(CC) -fPIC -o $@ -c $< ${CFLAGS}

clean:
	-rm ${OBJ} ${BINARY} ${LIBNAME} ${SONAME}

