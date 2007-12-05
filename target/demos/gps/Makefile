INSTALL_DIR="../../../buildroot/project_build_arm/armadeus/root/usr/bin/"
CC="../../../buildroot/build_arm/staging_dir/bin/arm-linux-gcc"
#CFLAGS+="-g"
CFLAGS+="-Wall"

default: nmea_decode

all: nmea_decode

nmea_decode: nmea_decoder.c
	$(CC) $(CFLAGS) -o $@ $^

install: nmea_decode
	cp nmea_decode  $(INSTALL_DIR)

clean: 
	rm -f nmea_decode
	rm -f *.o
