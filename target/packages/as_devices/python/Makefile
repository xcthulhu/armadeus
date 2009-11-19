# to compile c tests
# make CC=~/armadeus_trees/apf27/buildroot/build_armv5te/staging_dir/usr/bin/arm-linux-gcc 
#

CFLAGS=-Wall -g -O0
LDFLAGS=

INCLUDE_LIB=../c/

AS_LIB=../c/libas_devices.so

LIBNAME=AsDevices

WRAPPER_LIBS=Apf27Pwm_wrap.so
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
