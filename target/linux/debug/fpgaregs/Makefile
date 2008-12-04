ifeq ($(CC),cc)
        CC=arm-linux-gcc
endif


all: fpgaregs

fpgaregs: fpgaregs.c
	$(CC) -mcpu=arm920t -Wall fpgaregs.c -o $@
    
clean:
	rm -f *.o
	rm -f fpgaregs
