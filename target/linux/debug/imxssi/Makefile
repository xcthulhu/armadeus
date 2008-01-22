
all: imxssi

imxssi: imxssi.c imxregs.h
	arm-linux-gcc imxssi.c -O2 -lm -o $@
    
clean:
	rm -f *.o
	rm -f imxssi
