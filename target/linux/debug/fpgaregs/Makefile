
all: fpgaregs

imxregs: fpgaregs.c
	arm-linux-gcc fpgaregs.c -o $@
    
clean:
	rm -f *.o
	rm -f fpgaregs
