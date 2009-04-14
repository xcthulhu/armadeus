ifeq ($(CC),cc)
        CC=arm-linux-gcc
endif

all: fpgaregs

fpgaregs: fpgaregs.c fpgaregs.h 
	$(CC) -Wall fpgaregs.c -o $@

fpgaregs: fpgaregs.c fpga27regs.h
	$(CC) -DIMX27 -Wall fpgaregs.c -o $@
    
clean:
	rm -f *.o
	rm -f fpgaregs
