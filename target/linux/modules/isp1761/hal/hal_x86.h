/************************************************************
 * NXP ISP176x Hardware Access Interface header file
 *
 * (c) 2006 NXP B.V., All rights reserved. <usb.linux@nxp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * File Name: hal_x86.h
 *
 * Refering linux kernel version 2.6.9
 *
 * History:
 *
 * Date                Author                  Comments
 * ---------------------------------------------------------------------
 * Nov 29 2005        Prabhakar Kalasani      Initial Creation     
 *
 **********************************************************************
 */

#ifndef _HAL_X86_H_
#define _HAL_X86_H_

#define         DRIVER_AUTHOR          "NXP Semiconductors"
#define         DRIVER_DESC            "ISP1761 bus driver"

/* Driver tuning, per NXP requirements: */

/* BIT defines */
#define BIT0    (1 << 0)
#define BIT1    (1 << 1)
#define BIT2    (1 << 2)
#define BIT3    (1 << 3)
#define BIT4    (1 << 4)
#define BIT5    (1 << 5)
#define BIT6    (1 << 6)
#define BIT7    (1 << 7)
#define BIT8    (1 << 8)
#define BIT9    (1 << 9)
#define BIT10   (1 << 10)
#define BIT11   (1 << 11)
#define BIT12   (1 << 12)
#define BIT13   (1 << 13)
#define BIT14   (1 << 14)
#define BIT15   (1 << 15)
#define BIT16   (1 << 16)
#define BIT17   (1 << 17)
#define BIT18   (1 << 18)
#define BIT19   (1 << 19)
#define BIT20   (1 << 20)
#define BIT21   (1 << 21)
#define BIT22   (1 << 22)
#define BIT23   (1 << 23)
#define BIT24   (1 << 24)
#define BIT25   (1 << 26)
#define BIT27   (1 << 27)
#define BIT28   (1 << 28)
#define BIT29   (1 << 29)
#define BIT30   (1 << 30)
#define BIT31   (1 << 31)

/* Definitions Related to Little Endian & Big Endian */
#define tole(x) __constant_cpu_to_le32(x)
#define tobe(x) __constant_cpu_to_be32(x)

/* Definitions Related to Chip Address and CPU Physical Address 
 * cpu_phy_add: CPU Physical Address , it uses 32 bit data per address
 * chip_add   : Chip Address, it uses double word(64) bit data per address
 */
#define chip_add(cpu_phy_add)  (cpu_phy_add-0x400)/8
#define cpu_phy_add(chip_add)  (8*chip_add)+0x400

/* for getting end add, and start add, provided we have one address with us */
/* IMPORTANT length  hex(base16) and dec(base10) works fine*/
#define end_add(start_add,length) (start_add+(length-4))
#define start_add(end_add,length) (end_add-(length-4))

/* Device Registers*/
#define DEV_UNLOCK_REGISTER             0x27C
#define DEV_INTERRUPT_REGISTER          0x218


/* The QHA Data Structure */
struct phci_qh {
    u32                 hw_info1; /* see QHA  W0, the first 32bits */
    u32                 hw_info2; /* see QHA  W1 */
    u32                 hw_info3; /* see QHA  W2 */
    u32                 hw_info4; /* see QHA  W3 */
    u32                 Reserved1; /* Reserved1..4. NOT in USE */
    u32                 Reserved2;
    u32                 Reserved3;
    u32                 Reserved4;
}phci_qh  __attribute__ ((aligned (32)));


/*
 *  We have hardware support for asynchronous DMA transfers.  We can use PIO
 *  mode as well.  The chips registers are mapped as follows:
 *
 *  Base address for pio mode: ioremap_nocache(PXA_CS2_BASE,PAGE_SIZE);
 *  Physical addresses for DMA transfers: 
 *  Note also that these are normally defined in the platform specific header
 *  files.  DON'T define them within your driver, that's extremely non-portable.
 */  
typedef struct isp1761_hal {
    void    *mem_base; /* Memory base */
    __u8        irq_usage;      /* NUmber of drivers using INT channel */
} isp1761_hal_t;

typedef struct {
    char *name;                                 /* dma stream identifier */
    char *data_buffer;                  /* pointer to in memory dma buffer (virtual) */
    char *phys_data_buffer;             /* pointer to in memory dma buffer (physical) */
    char *trash_buffer;                 /* don't ask... */
    char *phys_trash_buffer;
    u_int dma_ch;                               /* DMA channel number */
    volatile u32 *drcmr;                /* the dma request channel to use, not to be confused with the above */
    u_long dcmd;                                /* DMA dcmd field */
    u_long dev_addr;                    /* device physical address for this channel */
    u_long blklen;                              /* block length of specific transfer */
    char output;                                /* 0 for input, 1 for ouput */
} isp1761_channel_t;

typedef struct {
    isp1761_channel_t *dreq0;
    isp1761_channel_t *dreq1;
    struct semaphore sem;       /* may be handy for races... */
} dma_state_t;

#endif /*_HAL_X86_H_ */
