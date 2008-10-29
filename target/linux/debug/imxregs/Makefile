ifeq ($(CC),cc)
	CC=arm-linux-gcc
endif

all: imxregs

imxregs: imxregs.c imxregs.h
	$(CC) imxregs.c -o $@
    
clean:
	rm -f *.o
	rm -f imxregs
