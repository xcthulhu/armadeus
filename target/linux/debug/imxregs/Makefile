CC=arm-linux-gcc


all: imxregs imx27regs

imxregs: imxregs.c imxregs.h definitions.h
	$(CC) imxregs.c -o $@

imx27regs: imxregs.c imx27regs.h definitions.h
	$(CC) -DIMX27 imxregs.c -o $@

clean:
	rm -f *.o
	rm -f imxregs imx27regs
