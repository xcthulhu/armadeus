/*
 * imxssi.c - Tool to send SSI data from Linux's userspace
 *
 * Maintainers: E. Jarrige, J. Boibessot
 * (c) Copyright 2008 by Armadeus Systems
 * Derivated from pxaregs (c) Copyright 2002 by M&N Logistik-Loesungen Online GmbH
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
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>


//#define DEBUG TRUE

#ifdef IMXL
#include "imxregs.h"
#define SSI_TRANSMIT_DATA_REG    0x00218000
#define SSI_FIFO_STATUS_REGISTER 0x00218020
#else
#include "imx27regs.h"
#define SSI_TRANSMIT_DATA_REG    0x10010000
#define SSI_FIFO_STATUS_REGISTER 0x1001002C
#endif
#define TFCNT_MASK               0x0F00
#define TFCNT_SHIFT              8


// fd for /dev/mem
static int fd = -1;
#define TEST_SIGNAL_MAX_SAMPLES 130712
static int *table;
// static int ttable[130712];

// Get value of the register at the given address
static int getmem(u32 addr)
{
    void *map, *regaddr;
    u32 val;
    #ifdef DEBUG
    printf("getmem(0x%08x)\n", addr);
    #endif // DEBUG
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd<0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK );
    if (map == (void*)-1 ) {
        perror("mmap()");
        exit(1);
    }
    #ifdef DEBUG
    printf("Mapped addr: 0x%x\n", regaddr);
    #endif // DEBUG
    regaddr = map + (addr & MAP_MASK);
    
    val = *(u32*) regaddr;
    munmap(0,MAP_SIZE);
    
    return val;
}

// Modify register value at given address
static void putmem(u32 addr, u32 val)
{
    void *map, *regaddr;
    static int fd = -1;
    
    //printf("putmem(0x%08x, 0x%08x)\n", addr, val);
    
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd<0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK );
    if (map == (void*)-1 ) {
        perror("mmap()");
        exit(1);
    }
    printf("Mapped addr: 0x%x\n", regaddr);
    regaddr = map + (addr & MAP_MASK);
    
    *(u32*) regaddr = val;
    munmap(0,MAP_SIZE);
}

//
// Set content of register with given name
static void setreg(char *name, u32 val)
{
    int i;
    u32 mem;
    int found=0;
    int count=0;
    int n=sizeof(regs)/sizeof(struct reg_info);
    
    for (i=0; i<n; i++) {
        if (strcmp(regs[i].name, name)==0) {
            found = i;
            //printf("Matched %s with %s, count=%d\n", regs[i].name, name, count);
            count++;
        }
    }
    if (count!=1) {
        printf("No or more than one matching register found\n");
        exit(1);
    }
    
    mem = getmem(regs[found].addr);
    //printf("Old contents: 0x%08x\n", mem);
    mem &= ~(regs[found].mask << regs[found].shift);
    //printf("Unmasked contents: 0x%08x\n", mem);
    val &= regs[found].mask;
    //printf("mask: 0x%08x\n", regs[found].mask);
    //printf("masked val: 0x%08x\n", val);
    mem |= val << regs[found].shift;
    //printf("Embedded value: 0x%08x\n", mem);
    putmem(regs[found].addr, mem);
}

static void audio(u32 type, u32 samples, float level)
{
    volatile void *map, *regaddr, *dma;
    static int fd = -1;
    u32 counter = 0;
    u32 i;

    table = malloc(TEST_SIGNAL_MAX_SAMPLES*sizeof(*table));
    if (!table) {
        printf("Not enough memory !\n");
        exit(1);
    }

    if (samples > 65536) samples = 65536;
    if (-1.0f > level) -1.0f;
    if ( 1.0f < level) 1.0f;
    
//     for (i=0;i<65536;i++)
//         ttable[i] = 0;
    
    if (0 == (type%2)) {
        u32 i=0;
        for (i=0; i<(samples/2); i++) {
            table[i] = roundf((-32768+ ((i*(65535/samples))*2*level)));
            table[samples-1-i] = roundf(((-32768 + ((i*(65535/samples))*2*level))));
//              printf("tab[%d]=%x tab[%d]=%x\n", i, table[i], samples-1-i, table[samples-1-i]);
        }
    } else {
        u32 i=0;
        double cosx;
        for (i=0;i<samples;i++) {
            cosx = cos(((double)i/samples)*2*M_PI);
            table[i] = roundf(cosx*32767*level);
        }
    }
    
    //printf("putmem(0x%08x, 0x%08x)\n", SSI_TRANSMIT_DATA_REG, val);
    
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd<0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SSI_TRANSMIT_DATA_REG & ~MAP_MASK );
    if (map == (void*)-1 ) {
        perror("mmap() ssi");
        exit(1);
    }
    printf("Mapped ssi addr: 0x%x\n", map);
    regaddr = map + (SSI_TRANSMIT_DATA_REG & MAP_MASK);

#ifdef USE_DMA
    dma = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00209000 & ~MAP_MASK );
    if (dma == (void*)-1 ) {
        perror("mmap() dma");
        exit(1);
    }
    printf("Mapped dma addr: 0x%x\n", dma);
    *(volatile u32*)(dma+0x2CC) = 0x20A0; /* CCR9 */
#endif

    if (type > 1) {
#ifdef USE_DMA
        *(volatile u32*)(dma+0x2CC) = 0x2000; /* CCR9 */
        *(volatile u32*)(dma+0x2C0) = table; /* SAR9 */
//         *(volatile u32*)(dma+0x2C4) = ttable/*SSI_TRANSMIT_DATA_REG*/; /* DAR9 */
        *(volatile u32*)(dma+0x2C8) = samples; /* CNTR9 */
        *(volatile u32*)(dma+0x2CC) = 0x2000; /* CCR9 */
        *(volatile u32*)(dma+0x2D0) = 16; /* RSSR9 */
        *(volatile u32*)(dma+0x2D4) = 16; /* BLR9 */
        *(volatile u32*)(dma+0x2CC) = 0x000F; /* CCR9 */
#endif
        while(1) {
            while(6 < ((*(volatile u32*)(map + (SSI_FIFO_STATUS_REGISTER & MAP_MASK))) & TFCNT_MASK) >> TFCNT_SHIFT );
            
            {
                u32 i;
                for (i=0;i<samples;i++)
                    printf("tab[%d]=%d tab[%d]=%d\n", i, table[i], i, /*t*/table[i]);
                /**(volatile short*) regaddr = 32000;*/
                /*	  *(volatile short*) regaddr = table[counter&0X0FFFF];
                    *(volatile short*) regaddr = table[counter&0X0FFFF];
                    counter = (counter+1)%samples;
                */
            }
        }
    } else {
        while(1) {
            /* Wait 2 empty spaces in Transmit FIFO */
            while(6 < ((*(volatile u32*)(map + (SSI_FIFO_STATUS_REGISTER & MAP_MASK))) & TFCNT_MASK) >> TFCNT_SHIFT );

            *(volatile u32*) regaddr = table[counter & 0x0FFFF];
            *(volatile u32*) regaddr = table[counter & 0x0FFFF];
            counter = (counter+1) % samples;
        }
    }

    free(table);
    munmap(0,MAP_SIZE);
}


/////////////////////////////////////////////////////////////////////////////////////
// MAIN
//
int main(int argc, char *argv[])
{
    char *p;
    u32 val;

    if (argc == 4) {
        u32 type;
        float level;

#ifdef IMXL
        setreg("SFCSR", 0x0088);
        setreg("STCCR", 0x6103);
        setreg("SRCCR", 0x6103);
        setreg("STCR",  0x028D);
        setreg("SCSR",  0xDB41);
#else
        setreg("PCCR0",   0xd5010013);
        setreg("PCCR1",   0xa21a0e30);
        setreg("PPCR1",   0x00001000);
        setreg("HPCR1",   0xcc007000);
        setreg("STCR_1",  0x0000028d); /* I2S mode */
        setreg("SRCR_1",  0x0000028d); /* I2S mode */
        setreg("STCCR_1", 0x0000e000);
        setreg("SCR_1",   0x00000057); /* I2S mode */
#endif

        sscanf(argv[1],"%i",&type);
        sscanf(argv[2],"%i",&val);
        sscanf(argv[3],"%f",&level);
        audio(type, val, level);

        return 0;
    }
    // In all other cases, print Usage
    printf("Version : " VERSION "\n");
    printf("Usage: %s <signal_type> <samples> <scale> - to audio signal:\n"
           "          <signal_type> 0:triangular 1:sinusoid\n"
           "          <samples> nb sample per period\n"
           "          <scale>  0.0 .. 1.0 signal amplitude\n"
           "    !! tsc210x has to be configured in master mode !!\n",
        argv[0], argv[0], argv[0], argv[0]);

    return 1;
}
