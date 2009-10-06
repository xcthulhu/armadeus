# to compile for arm : 
# make CXX=../../bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-g++
# make CXX=~/Projects/HMSDemo/software/bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-g++
# make CXX=~/Projects/virgin_HMS/software/bsp/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-g++
# 
CXXFLAGS=-Wall -g -O0
LDFLAGS=

OBJ=as_apf27_pwm.o
OBJ+= as_max1027.o 
OBJ+= as_max5821.o 
OBJ+= as_adc.o 
OBJ+= as_dac.o 
OBJ+= as_apf27_gpio.o 
OBJ+= as_ad5258.o
OBJ+= as_i2c.o
OBJ+= as_ds1374.o

LIBRARY=as_devices_cpp
LIB_VERS=1
STAT_LIB=${LIBRARY}.a
SONAME=lib${LIBRARY}.so
LIBNAME=${SONAME}.${LIB_VERS}
ARCHIVE_CMD=ar
ARCHIVE_OPTS=-cvq
C_INCLUDE=../c/
C_LIB=../c/libas_devices.so.1

${LIBNAME}: ${OBJ}
	${CXX} -shared -Wl,-soname,${LIBNAME} -o ${LIBNAME} ${OBJ} -lc ${C_LIB} 
	ln -s ${LIBNAME} ${SONAME} 

${STAT_LIB}: ${OBJ} 
	${ARCHIVE_CMD} ${ARCHIVE_OPTS} ${STAT_LIB} ${OBJ}

%.o: %.cpp
	$(CXX) -fPIC -o $@ -c $< ${CXXFLAGS} -I${C_INCLUDE}

clean:
	-rm ${OBJ} ${BINARY}
	-rm ${LIBNAME} ${SONAME}
