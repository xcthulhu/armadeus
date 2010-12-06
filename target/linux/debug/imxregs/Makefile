CC=arm-linux-gcc
EXECS:= imxregs imx27regs imx51regs


all: $(EXECS)

imxregs: imxregs.c imxregs.h definitions.h
	$(CC) imxregs.c -o $@

imx27regs: imxregs.c imx27regs.h definitions.h
	$(CC) -DIMX27 imxregs.c -o $@

imx51regs: imxregs.c imx51regs.h definitions.h
	$(CC) -DIMX51 imxregs.c -o $@

clean:
	rm -f *.o
	rm -f $(EXECS)
