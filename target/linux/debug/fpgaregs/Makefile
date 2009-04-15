ifeq ($(CC),cc)
        CC=arm-linux-gcc
endif

all: fpgaregs fpga27regs 

fpgaregs: fpgaregs.c fpgaregs.h 
	$(CC) -Wall fpgaregs.c -o $@

fpga27regs: fpgaregs.c fpga27regs.h
	$(CC) -DIMX27 -Wall fpgaregs.c -o $@
    
clean:
	rm -f *.o
	rm -f fpgaregs
	rm -f fpga27regs
