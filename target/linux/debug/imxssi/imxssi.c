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

/*#include <linux/i2c.h>
#include <linux/i2c-dev.h>*/

#include "../imxregs/imxregs.h"

#define DEBUG TRUE

// fd for /dev/mem
static int fd = -1;
static int table[130712];
static int ttable[130712];

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

static u32 lastaddr = 0;
static u32 newaddr = 1;
static u32 data = 0;
static u32 shiftdata;

//
// Show content of given register (index in reginfo struct)
static void dumpentry(int i)
{
    int j;
    
    if (regs[i].addr != lastaddr) newaddr = 1;
    if (newaddr) {
        newaddr = 0;
        lastaddr = regs[i].addr;
        data = getmem(lastaddr);
        printf("\n%s\n", regs[i].desc);
        printf("%-24s 0x%08x  -> ", regs[i].name, data);
        shiftdata = data;
        for (j=32; j>0; j--) {
            printf("%c", shiftdata & 0x80000000 ? '1' : '0');
            shiftdata = shiftdata << 1;
            if (j==9 || j==17 || j==25) printf(" ");
        }
    
        printf("\n");
    }
    
    if( (regs[i].shift != 0)  || (regs[i].mask != 0xffffffff) ) {
        shiftdata = (data >> regs[i].shift) & regs[i].mask;
        printf("%-25s  ", regs[i].name);
        switch (regs[i].type)
        {
            // Hexa printing
            case 'x':
                printf("%8x", shiftdata);
            break;
            case '<':
                printf("%8u", 1 << shiftdata);
            break;
            // Decimal printing
            default:
                printf("%8u", shiftdata);
            break;
        }
        printf("  %s\n", regs[i].desc);
    }
}

//
// Show content of all supported registers
static void dumpall(void)
{
    int i;
    int n=sizeof(regs)/sizeof(struct reg_info);
    
    for( i=0; i<n; i++ ) {
        dumpentry(i);
    }
}

//
// Show content of register with name starting with given string
static void dumpmatching(char *name)
{
    int i;
    int n=sizeof(regs)/sizeof(struct reg_info);
    
    for (i=0; i<n; i++) {
        if (strstr(regs[i].name, name))
            dumpentry(i);
    }
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

    if (samples > 65536) samples = 65536;
    if (-1.0f > level) -1.0f;
    if ( 1.0f < level) 1.0f;
    
    for (i=0;i<65536;i++)
        ttable[i] = 0;
    
    if (0 == (type%2)) {
        u32 i=0;
        for (i=0;i<(samples/2);i++) {
            table[i] = roundf((-32768+ ((i*(65535/samples))*2*level)));
            table[samples-1-i] = roundf(((-32768 + ((i*(65535/samples))*2*level))));
            /*printf("tab[%d]=%d tab[%d]=%d\n", i, table[i], (65536/inc)-1-i, table[(65536/inc)-1-i]);*/
        }
    } else {
        u32 i=0;
        double cosx;
        for (i=0;i<samples;i++) {
            cosx = cos(((double)i/samples)*2*M_PI);
            table[i] = roundf(cosx*32767*level);
        }
    }
    
    //printf("putmem(0x%08x, 0x%08x)\n", 0x00218000, val);
    
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd<0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00218000 & ~MAP_MASK );
    if (map == (void*)-1 ) {
        perror("mmap() ssi");
        exit(1);
    }
    printf("Mapped ssi addr: 0x%x\n", map);
    regaddr = map + (0x00218000 & MAP_MASK);

    dma = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00209000 & ~MAP_MASK );
    if (dma == (void*)-1 ) {
        perror("mmap() dma");
        exit(1);
    }
    printf("Mapped dma addr: 0x%x\n", dma);
    
    *(volatile u32*)(dma+0x2CC) = 0x20A0; /* CCR9 */

    if (1 < type) {
        *(volatile u32*)(dma+0x2CC) = 0x2000; /* CCR9 */
        *(volatile u32*)(dma+0x2C0) = table; /* SAR9 */
        *(volatile u32*)(dma+0x2C4) = ttable/*0x00218000*/; /* DAR9 */
        *(volatile u32*)(dma+0x2C8) = samples; /* CNTR9 */
        *(volatile u32*)(dma+0x2CC) = 0x2000; /* CCR9 */
        *(volatile u32*)(dma+0x2D0) = 16; /* RSSR9 */
        *(volatile u32*)(dma+0x2D4) = 16; /* BLR9 */
        *(volatile u32*)(dma+0x2CC) = 0x000F; /* CCR9 */
        
        while(1) {
            while(6 < ((*(volatile u32*)(map + (0x00218020 & MAP_MASK)))&0x0F00)>>8 );
            
            {
                u32 i;
                for (i=0;i<samples;i++)
                    printf("tab[%d]=%d tab[%d]=%d\n", i, table[i], i, ttable[i]);
                /**(volatile short*) regaddr = 32000;*/
                /*	  *(volatile short*) regaddr = table[counter&0X0FFFF];
                    *(volatile short*) regaddr = table[counter&0X0FFFF];
                    counter = (counter+1)%samples;
                */
            }
        }
    } else {
        while(1) {
            while(6 < ((*(volatile u32*)(map + (0x00218020 & MAP_MASK)))&0x0F00)>>8 );
            
            {
                *(volatile u32*) regaddr = table[counter&0X0FFFF];
                *(volatile u32*) regaddr = table[counter&0X0FFFF];
                counter = (counter+1)%samples;
            }
        }
    }
    
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

        setreg("SFCSR", 0x0088);
        setreg("STCCR", 0x6103);
        setreg("SRCCR", 0x6103);
        setreg("STCR", 0x028D);
        setreg("SCSR", 0xDB41);

        sscanf(argv[1],"%i",&type);
        sscanf(argv[2],"%i",&val);
        sscanf(argv[3],"%f",&level);
        audio(type, val, level);
        return 0;
    }
    // In all other cases, print Usage
    printf("Usage: %s <signal_type> <samples> <scale> - to audio signal:\n"
           "          <signal_type> 0:triangular 1:sinusoid\n"
           "          <samples> nb sample per period\n"
           "          <scale>  0.0 .. 1.0 signal amplitude\n"
           "    !! tsc210x has to be configured in master mode !!\n",
        argv[0], argv[0], argv[0], argv[0]);

    return 1;
}
