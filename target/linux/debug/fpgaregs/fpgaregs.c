/************************************************************************
 *
 * fpgaregs.c
 * a program to write/read 16/32 bits values on FPGA address map using mmap()
 *
 * (c) Copyright 2008 Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Modified by Sebastien Van Cauwenberghe <svancau@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>

/* file management */
#include <sys/stat.h>
#include <fcntl.h>

/* as name said */
#include <signal.h>

/* sleep, write(), read() */
#include <unistd.h>

/* converting string */
#include <string.h>

/* memory management */
#include <sys/mman.h>

#ifdef IMX27
#	include "fpga27regs.h"
#else
#	include "fpgaregs.h"
#endif

#define WORD_ACCESS (2)
#define LONG_ACCESS (4)


void displayResult(char* text, unsigned int accesstype, unsigned int value, unsigned int address)
{
	if (accesstype == WORD_ACCESS)
		printf("%s %04x at %08x\n", text, (unsigned short)value, address);
	else
		printf("%s %08x at %08x\n", text, value, address);
}


int main(int argc, char *argv[])
{
	unsigned short address;
	unsigned int value;
	int ffpga, accesstype = LONG_ACCESS;
	void* ptr_fpga;

	if ((argc < 3) || (argc > 4)) {
		printf("invalid arguments number\n");
		printf("fpgaregs for platform %s\n",PLATFORM);
		printf("Usage:\n");
		printf("fpgaregs [w,l] fpga_reg_add [value].\n");
		printf("        w: word access\n");
		printf("        l: long access\n");
		printf("\n          Ex: fpgaregs w 0x10 0x1234, fpgaregs l 0x10 0x12345678\n");
		return -1;
	}

	ffpga = open("/dev/mem", O_RDWR|O_SYNC);
	if (ffpga < 0) {
		printf("can't open file /dev/mem\n");
		return -1;
	}

	ptr_fpga = mmap(0, 8192, PROT_READ|PROT_WRITE, MAP_SHARED, ffpga, FPGA_ADDRESS);
	if (ptr_fpga == MAP_FAILED) {
		printf("mmap failed\n");
		return -1;
	}

	address = (unsigned int)strtol(argv[2], (char **)NULL, 16);

	if (argc < 3) {
		printf("invalid command line");
	} else {
		if (*(argv[1]) == 'w')
			accesstype = WORD_ACCESS;
		if (accesstype == WORD_ACCESS) {
			if (address%2 != 0) {
				printf("Can't read word in even address (%d)\n", address);
				return -1;
			}
		} else { /* LONG_ACCESS */
			if (address%4 != 0) {
				printf("Can't read long in no-4-multiple address (%d)\n", address);
				return -1;
			}
		}

		/* write value at address */
		if (argc == 4) {
			value = strtoul(argv[3], (char **)NULL, 16);
			if (accesstype == WORD_ACCESS)
				*(unsigned short*)(ptr_fpga+(address)) = (unsigned short)value;
			else
				*(unsigned int*)(ptr_fpga+(address)) = (unsigned int)value;

			displayResult("Write", accesstype, value, address);

			/* read address value */
		} else if (argc == 3) {
			if (accesstype == WORD_ACCESS)
				value = *(unsigned short*)(ptr_fpga+(address));
			else
				value = *(unsigned int*)(ptr_fpga+(address));

			displayResult("Read", accesstype, value, address);
		}
	}
	close(ffpga);
	return 0;
}
