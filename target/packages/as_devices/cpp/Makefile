# to compile for arm : 
# make CXX=../../../buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-g++
# 
# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../../
-include $(ARMADEUS_BASE_DIR)/Makefile.in
CXXFLAGS=-Wall -g -O0
LDFLAGS=

OBJS = as_i2c.o
OBJS += as_gpio.o
OBJS += as_dac.o
OBJS += as_adc.o

OBJS += as_spi.o

LIBRARY=as_devices_cpp
LIB_VERS=1
STAT_LIB=${LIBRARY}.a
SONAME=lib${LIBRARY}.so
LIBNAME=${SONAME}.${LIB_VERS}
ARCHIVE_CMD=ar
ARCHIVE_OPTS=-cvq

C_INCLUDE=../c/
C_LIB=../c/libas_devices.so.1

${LIBNAME}: ${OBJS}
	${CXX} -shared -Wl,-soname,${SONAME} -o ${SONAME} ${OBJS} ${C_LIB} -lc 
	ln -s ${SONAME} ${LIBNAME} 

${STAT_LIB}: ${OBJS} 
	${ARCHIVE_CMD} ${ARCHIVE_OPTS} ${STAT_LIB} ${OBJS}

%.o: %.cpp
	$(CXX) -fPIC -o $@ -c $< ${CXXFLAGS} -I${C_INCLUDE} -D__cplusplus

clean:
	-rm ${OBJ} ${BINARY} ${STAT_LIB} ${LIBNAME}
	-rm *.o
	-rm *.so

