# to compile for arm : 
# make CC=../../bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-gcc
# make CC=~/Projects/HMSDemo/software/bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-gcc
# make CC=~/Projects/virgin_HMS/software/bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-gcc
# 
CFLAGS=-Wall -g -O0
LDFLAGS=

OBJ=as_apf27_pwm.o
OBJ+= as_i2c.o

LIBRARY=as_devices
LIB_VERS=1
STAT_LIB=${LIBRARY}.a
SONAME=lib${LIBRARY}.so
LIBNAME=${SONAME}.${LIB_VERS}
ARCHIVE_CMD=ar
ARCHIVE_OPTS=-cvq

${LIBNAME}: ${OBJ}
	${CC} -shared -Wl,-soname,${SONAME} -o ${SONAME} ${OBJ} -lc 
	ln -s ${SONAME} ${LIBNAME}

${STAT_LIB}: ${OBJ} 
	${ARCHIVE_CMD} ${ARCHIVE_OPTS} ${STAT_LIB} ${OBJ}

%.o: %.c
	$(CC) -fPIC -o $@ -c $< ${CFLAGS}

clean:
	-rm ${OBJ} ${BINARY} ${LIBNAME} ${SONAME}

