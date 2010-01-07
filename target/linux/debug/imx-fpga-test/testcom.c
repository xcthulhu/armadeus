/************************************************************************
 * a program to test communication between imx and fpga
 * 6 january 2010 
 *
 * (c) Copyright 2010 Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
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

/* time */
#include <time.h>
#include <sys/times.h>

#define FPGA_ADDRESS (0xD6000000)
#define FPGA_SIZE    (0xffffff)

#define FPGA

#define RAM1_OFFSET (0x1000)
#define RAM_SIZE    (2048)
#define RAM_SIZE_W  (RAM_SIZE/2)
#define MBYTE       (1000000)

#define ACCESS_NUMBER (2000*100)

int write_2kB(void* ptr_fpga, void* tab)
{
	memcpy(ptr_fpga, tab, RAM_SIZE);
	return 1;
}

int read_2kB(void* ptr_fpga, void *value)
{
	memcpy(value, ptr_fpga, RAM_SIZE);
	return 1;
}

/* 32bits:int, 16bits:short, 8bits:char */
#define TYPE unsigned int

int main(int argc, char *argv[])
{
    int ffpga;
    int i, j;
    clock_t tick1, tick2;
    float duration_s;
    int tick;
    TYPE * ptr_fpga;

    unsigned short value_tab[RAM_SIZE_W];
    unsigned short value_tab2[RAM_SIZE_W];

    unsigned char  value_tab_c[RAM_SIZE];
    unsigned char  value_tab2_c[RAM_SIZE];

    unsigned int value_tab_i[RAM_SIZE_W/2];
    unsigned int value_tab2_i[RAM_SIZE_W/2];

    memset(value_tab, 0x55, RAM_SIZE);
    memset(value_tab_c, 0x55, RAM_SIZE);
    memset(value_tab_i, 0x55, RAM_SIZE);

    value_tab[0]   = 0x1234;
    value_tab[1]   = 0x5678;
    value_tab[1023] = 0x9876;

    value_tab_c[0]   = 0x12;
    value_tab_c[1]   = 0x56;
    value_tab_c[1023] = 0x98;

    value_tab_i[0]    = 0x12345678l;
    value_tab_i[1]    = 0x56789012l;
    value_tab_i[1023] = 0x98765432l;

#ifdef FPGA
    printf("******************\n");
    printf("* test on FPGA   *\n");
    printf("******************\n");
    printf(" Please, do not forget to load BRAMtest IP (in firmware/BRAMtest/bin)\n");
#else
    printf("******************\n");
    printf("* test on memory *\n");
    printf("******************\n");
#endif

#ifdef FPGA 
    /* open memory map */
    ffpga=open("/dev/mem",O_RDWR|O_SYNC);
    if(ffpga<0){
        printf("can't open file /dev/mem\n");
        return -1;
    }

    /* map memory (all fpga domain)*/
    ptr_fpga = mmap (0,FPGA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, ffpga, FPGA_ADDRESS);
    if (ptr_fpga == MAP_FAILED)
    {
        printf ("mmap failed\n");
        return -1;
    }
#else
    ptr_fpga = (void *)malloc(RAM_SIZE*sizeof(unsigned char));
    if(ptr_fpga == NULL)
    {
	    printf("Error can't malloc memory\n");
	    exit(-1);
    }
#endif

    printf("\n I - Testing communication with memcpy() and without check\n\n");
    tick1 = times(NULL);
    for (i=0; i < ACCESS_NUMBER; i++)
    	write_2kB(ptr_fpga, value_tab);
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);
	
    printf("Wrote %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((ACCESS_NUMBER*RAM_SIZE)/duration_s)/MBYTE);

    tick1 = times(NULL);
    for (i=0; i < ACCESS_NUMBER; i++)
    	read_2kB(ptr_fpga, value_tab);
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);
	
    printf("Read %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((ACCESS_NUMBER*RAM_SIZE)/duration_s)/MBYTE);

    printf("\n II - Testing communication with memcpy() and check result\n\n");
    tick1 = times(NULL);
    for (i=0; i < ACCESS_NUMBER; i++)
    {
    	write_2kB(ptr_fpga, value_tab);
    	read_2kB(ptr_fpga, value_tab2);
	if(memcmp(value_tab, value_tab2, RAM_SIZE))
	{
		printf("Error: values read are not equal to values wrote\n");
	}
    }
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);

    printf("This message is used to wake up max232, . . . . . , max232 awake.\n");
    printf("Wrote then read %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((2*ACCESS_NUMBER*RAM_SIZE)/duration_s)/MBYTE);


    printf("\n III- Testing communication with singles 16bits access (with check)\n\n");
    tick1 = times(NULL);
    for(i=0; i < ACCESS_NUMBER; i++)
	    for(j=0; j < RAM_SIZE_W; j++)
	    {
		*((unsigned short *)ptr_fpga + j) = value_tab[j];
		value_tab2[j] = *((unsigned short *)ptr_fpga + j);
		if (value_tab[j] != value_tab2[j])
			printf("Error value read %04X is different than value wrote %04X\n", value_tab2[j], value_tab[j]);
	    }
    /* TODO */
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);
    printf("This message is used to wake up max232, . . . . . , max232 awake.\n");
    printf("Wrote then read %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((2*ACCESS_NUMBER*RAM_SIZE)/duration_s)/MBYTE);

    printf("\n IV - Testing communication with singles 8bits access (with check)\n\n");

    tick1 = times(NULL);
    for(i=0; i < ACCESS_NUMBER; i++)
	    for(j=0; j < RAM_SIZE; j=j+2)
	    {
		*((unsigned char *)ptr_fpga + j) = value_tab_c[j];
		value_tab2_c[j] = *((unsigned char *)ptr_fpga + j);
		if (value_tab_c[j] != value_tab2_c[j])
			printf("Error value read %02X is different than value wrote %02X\n", value_tab2_c[j], value_tab_c[j]);
	    }
    /* TODO */
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);
    printf("This message is used to wake up max232, . . . . . , max232 awake.\n");
    printf("Wrote then read %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE_W,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((2*ACCESS_NUMBER*RAM_SIZE_W)/duration_s)/MBYTE);

    printf("\n V  - Testing communication with singles 32bits access (with check)\n\n");
    tick1 = times(NULL);
    for(i=0; i < ACCESS_NUMBER; i++)
	    for(j=0; j < RAM_SIZE_W/2; j++)
	    {
		*((unsigned int *)ptr_fpga + j) = value_tab_i[j];
		value_tab2_i[j] = *((unsigned int *)ptr_fpga + j);
		if (value_tab_i[j] != value_tab2_i[j])
			printf("Error value read %08X is different than value wrote %08X\n", value_tab2_i[j], value_tab_i[j]);
	    }
    /* TODO */
    tick2 = times(NULL);
    tick = (float)(tick2-tick1);
    duration_s = tick/((float)CLK_TCK);
    printf("This message is used to wake up max232, . . . . . , max232 awake.\n");
    printf("Wrote then read %dx%d Bytes in %g sec\n",
	   ACCESS_NUMBER,
	   RAM_SIZE,
	   duration_s);

    printf("Speed : %gMB/s\n",
	   (float)((2*ACCESS_NUMBER*RAM_SIZE)/duration_s)/MBYTE);


    close(ffpga);
    return 0;
}

