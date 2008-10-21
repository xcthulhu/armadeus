CC=../../../../buildroot/build_arm/staging_dir/bin/arm-linux-gcc

all: fpgaregs

fpgaregs: fpgaregs.c
	$(CC) -Wall fpgaregs.c -o $@
    
clean:
	rm -f *.o
	rm -f fpgaregs
