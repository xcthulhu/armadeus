/*
 * imxssi.c - Tool to send SSI data from Linux's userspace
 *
 * Maintainers: E. Jarrige, J. Boibessot
 * (c) Copyright 2008-2011 by Armadeus Systems
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
#include <getopt.h>


#define debug(fmt, arg...) ({ if (show_debug) printf(fmt, ##arg);})

#ifdef IMXL
# include "imxregs.h"
# define SSI_TRANSMIT_DATA_REG    0x00218000
# define SSI_FIFO_STATUS_REGISTER 0x00218020
#elif IMX27
# include "imx27regs.h"
# define SSI_TRANSMIT_DATA_REG    0x10010000	/* FIFO 0 */
# define SSI_RECEIVE_DATA_REG     0x10010008
# define SSI_FIFO_STATUS_REGISTER 0x1001002C
#elif IMX51
# include "imx51regs.h"
# define SSI_TRANSMIT_DATA_REG    0x83fcc000	/* FIFO 0 */
# define SSI_RECEIVE_DATA_REG     0x83fcc008
# define SSI_FIFO_STATUS_REGISTER 0x83fcc02c
#else
#error "Undefined processor"
#endif

#define RFCNT_MASK               0xF000
#define TFCNT_MASK               0x0F00		/* FIFO 0 for i.MX27 */
#define RFCNT_SHIFT              12
#define TFCNT_SHIFT              8
#define TYPE_TRIANGLE            0
#define TYPE_SINUS               1
#define TYPE_LOOP                2

static int fd = -1;
#define TEST_SIGNAL_MAX_SAMPLES 130712
static int *table;
// static int ttable[130712];
static int show_debug = 0;

/* Get value of the register at the given address */
static int getmem(u32 addr)
{
    void *map, *regaddr;
    u32 val;

    debug("getmem(0x%08x)\n", addr);
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd < 0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK);
    if (map == (void*)-1) {
        perror("mmap()");
        exit(1);
    }
    debug("Mapped addr: 0x%x\n", regaddr);
    regaddr = map + (addr & MAP_MASK);
    
    val = *(u32*) regaddr;
    munmap(0, MAP_SIZE);
    
    return val;
}

/* Modify register value at given address */
static void putmem(u32 addr, u32 val)
{
    void *map, *regaddr;
    static int fd = -1;
    
    /* printf("putmem(0x%08x, 0x%08x)\n", addr, val); */
    
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd<0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK);
    if (map == (void*)-1) {
        perror("mmap()");
        exit(1);
    }
    debug("Mapped addr: 0x%x\n", regaddr);
    regaddr = map + (addr & MAP_MASK);
    
    *(u32*) regaddr = val;
    munmap(0, MAP_SIZE);
}

/* Set content of register with given name */
static void setreg(char *name, u32 val)
{
    int i;
    u32 mem;
    int found=0;
    int count=0;
    int n=sizeof(regs)/sizeof(struct reg_info);
    
    for (i=0; i<n; i++) {
        if (strcmp(regs[i].name, name) == 0) {
            found = i;
            count++;
        }
    }
    if (count != 1) {
        printf("No or more than one matching register found\n");
        exit(1);
    }
    
    mem = getmem(regs[found].addr);
    mem &= ~(regs[found].mask << regs[found].shift);
    val &= regs[found].mask;
    mem |= val << regs[found].shift;
    putmem(regs[found].addr, mem);
}

int num_states = 8;
char progress[8] = {
    '|', '/', '-', ' ', '|', '/', '-', ' ',
};

static void audio(u32 type, u32 samples, float level)
{
    volatile void *map, *STX0, *SRX0, *SFCSR, *dma;
    static int fd = -1;
    u32 counter = 0;
    u32 i;
    int state = 0;

    debug("type %d samples %d level %f\n", type, samples, level);

    table = malloc(TEST_SIGNAL_MAX_SAMPLES*sizeof(*table));
    if (!table) {
        printf("Not enough memory !\n");
        exit(1);
    }

    if (samples > 65536) samples = 65536;
    if (level < -1.0f) level = -1.0f;
    if (level > 1.0f) level = 1.0f;
    
/*     for (i=0;i<65536;i++)
         ttable[i] = 0; */
    
    if (type == TYPE_TRIANGLE) {
        for (i = 0; i < (samples/2); i++) {
            table[i] = roundf(-32768 + ((i*(65535/samples))*2*level));
            table[samples-1-i] = roundf(-32768 + ((i*(65535/samples))*2*level));
//              printf("tab[%d]=%x tab[%d]=%x\n", i, table[i], samples-1-i, table[samples-1-i]);
        }
    } else if (type == TYPE_SINUS) {
        double cosx;
        for (i = 0; i < samples; i++) {
            cosx = cos(((double)i/samples)*2*M_PI);
            table[i] = roundf(cosx*32767*level);
        }
    }
    
    if (fd == -1) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd < 0) {
            perror("open(\"/dev/mem\")");
            exit(1);
        }
    }
    
    map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SSI_TRANSMIT_DATA_REG & ~MAP_MASK);
    if (map == (void*)-1) {
        perror("mmap() ssi");
        exit(1);
    }
    printf("Mapped SSI reg addr: 0x%x\n", map);
    STX0 = map + (SSI_TRANSMIT_DATA_REG & MAP_MASK);
    SRX0 = map + (SSI_RECEIVE_DATA_REG & MAP_MASK);
    SFCSR = map + (SSI_FIFO_STATUS_REGISTER & MAP_MASK);

#ifdef USE_DMA
    dma = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00209000 & ~MAP_MASK);
    if (dma == (void*)-1) {
        perror("mmap() dma");
        exit(1);
    }
    printf("Mapped dma addr: 0x%x\n", dma);
    *(volatile u32*)(dma+0x2CC) = 0x20A0; /* CCR9 */
#endif

    if (type > 2) {
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
        while (1) {
            while (6 < ((*(volatile u32*)(map + (SSI_FIFO_STATUS_REGISTER & MAP_MASK))) & TFCNT_MASK) >> TFCNT_SHIFT);
            {
                for (i=0; i<samples; i++)
                    printf("tab[%d]=%d tab[%d]=%d\n", i, table[i], i, /*t*/table[i]);
                /**(volatile short*) regaddr = 32000;*/
                /*	  *(volatile short*) regaddr = table[counter&0X0FFFF];
                    *(volatile short*) regaddr = table[counter&0X0FFFF];
                    counter = (counter+1)%samples;
                */
            }
        }
#ifndef IMXL
    } else if (type == TYPE_LOOP) {
    /* Loop audio in to audio out (only possible on APF27Dev & APF51Dev) */
        while (1) {
            /* Wait until Rx FIFO is half full */
            while (((*(volatile u32*)(SFCSR) & RFCNT_MASK) >> RFCNT_SHIFT) < 4);
            /* then copy Rx FIFO to Tx one */
            for (i=0; i<4; i++)
                *(volatile u32*) STX0 = *(volatile u32*) SRX0;
        }
#endif
    } else {
    /* Ramp or sin ouput */
        while (1) {
            /* Wait 2 empty spaces in Transmit FIFO */
            while (6 < ((*(volatile u32*)(SFCSR)) & TFCNT_MASK) >> TFCNT_SHIFT);

            *(volatile u32*) STX0 = table[counter & 0x0FFFF];
            *(volatile u32*) STX0 = table[counter & 0x0FFFF];
            counter = (counter+1) % samples;
            if (counter == 0) {
                if (show_debug) {
                    printf("\r%c", progress[state]);
                    fflush(stdout);
                }
                state = (state + 1) % (num_states - 1);
            }
        }
    }

    free(table);
    munmap(0, MAP_SIZE);
}

static void usage(const char* prog_name)
{
    printf("Version: " VERSION "\n");
    printf("Usage: %s [-t type] [-l level] [-s samples]\n\n"
           "Options:\n"
           "        -h, --help    this help\n"
#ifndef IMXL
           "        -L, --loop    loop Audio In to Audio Out\n"
#endif
           "        -t type       0:triangular/ramp  1:sinusoid\n"
           "        -s samples    nb of samples per period\n"
           "        -l level      0.0 .. 1.0 signal amplitude\n"
           "        -d, --debug   shows debug informations\n\n"
           "    !! audio CODEC has to be configured in master mode !!\n",
        prog_name);

    exit(1);
}

int main(int argc, char *argv[])
{
    char *p;
    u32 val;
    u32 type;
    float level;

    int option, option_index = 0;
    static struct option long_options[] = {
        {"help", 0, 0, 'h'},
        {"loop", 0, 0, 'L'},
        {"debug", 0, 0, 'd'},
        {0, 0, 0, 0}
    };

    /* default values : */
    type = TYPE_TRIANGLE;
    val = 200;
    level = 0.2;

    opterr = 0;
    while ((option = getopt_long(argc, argv, "hLt:s:l:d", long_options, &option_index)) != -1)
    {
        switch (option)
        {
            case 't':
                type = atoi(optarg);
            break;

            case 's':
                val = atoi(optarg);
            break;

            case 'h':
                usage(argv[0]);
            break;

            case 'l':
                 level = atof(optarg);
            break;

            case 'L':
                type = TYPE_LOOP;
            break;

            case 'd':
                show_debug = 1;
            break;

            case '?':
                if ((optopt == 's') || (optopt == 't') || (optopt == 'l'))
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                usage(argv[0]);

            default:
                abort();
        }
    }

    /* Setup Audio Mux & SSI */
#ifdef IMXL
    setreg("SFCSR", 0x0088);
    setreg("STCCR", 0x6103);
    setreg("SRCCR", 0x6103);
    setreg("STCR", 0x028D);
    setreg("SCSR", 0xDB41);
#else
# ifdef IMX27
    /* Clock */
    setreg("PCCR0", 0xd5010013);
    setreg("PCCR1", 0xa21a0e30);
    /* Audio Mux */
    setreg("PPCR1", 0x00001000);
    setreg("HPCR1", 0xcc007000);
    /* SSI */
    setreg("STCR_1", 0x0000028d); /* I2S mode */
    setreg("SRCR_1", 0x0000028d); /* I2S mode */
    setreg("STCCR_1", 0x0000e000);
# elif IMX51
    /* Audio Mux (SSI1 <-> Port 3, synchronous) */
    setreg("PTCR1", 0x94800800);
    setreg("PDCR1", 0x00004000);
    setreg("PTCR3", 0x00000800);
    setreg("PDCR3", 0x00000000);
# endif
    setreg("SCR_1", 0x00000057); /* I2S mode, Tx & Rx on, SSI on */
#endif

    audio(type, val, level);

    return 0;
}

