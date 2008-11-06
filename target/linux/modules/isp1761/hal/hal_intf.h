/**********************************************************************
 * Philips ISP176x Hardware Access Interface header file
 *
 * (c) 2002 Koninklijke Philips Electronics N.V. All rights reserved. <usb.linux@philips.com>
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
 * File Name:   hal_intf.h
 *
 * History:     
 *
 * Date                Author                  Comments
 * ---------------------------------------------------------------------
 * Nov 29 2005        Prabhakar Kalasani      Initial Creation     
 * Nov 04 2007        NC (armadeus)           Add 2.6.23 compatibility               
 ***********************************************************************/

#ifndef __HAL_INTF_H__
#define __HAL_INTF_H__

/* Values for id_flags filed of isp1761_driver_t */
#define         ISP1761_HC                              0               /* Host Controller Driver */
#define         ISP1761_DC                              1               /* Device Controller Driver */
#define         ISP1761_OTG                             2               /* Otg Controller Driver */
#define         ISP1761_LAST_DEV                        (ISP1761_OTG+1)
#define         ISP1761_1ST_DEV                         (ISP1761_HC)

#define         HC_SPARAMS_REG                          0x04    /* Structural Parameters Register */
#define         HC_CPARAMS_REG                          0x08    /* Capability Parameters Register */

#define         HC_USBCMD_REG                           0x20    /* USB Command Register */
#define         HC_USBSTS_REG                           0x24    /* USB Status Register */
#define         HC_INTERRUPT_REG_EHCI                   0x28    /* INterrupt Enable Register */
#define         HC_FRINDEX_REG                          0x2C    /* Frame Index Register */

#define         HC_CONFIGFLAG_REG                       0x60    /* Conigured Flag  Register */
#define         HC_PORTSC1_REG                          0x64    /* Port Status Control for Port1 */

/*ISO Transfer Registers */
#define         HC_ISO_PTD_DONEMAP_REG                  0x130   /* ISO PTD Done Map Register */
#define         HC_ISO_PTD_SKIPMAP_REG                  0x134   /* ISO PTD Skip Map Register */
#define         HC_ISO_PTD_LASTPTD_REG                  0x138   /* ISO PTD Last PTD Register */

/*INT Transfer Registers */
#define         HC_INT_PTD_DONEMAP_REG                  0x140   /* INT PTD Done Map Register */
#define         HC_INT_PTD_SKIPMAP_REG                  0x144   /* INT PTD Skip Map Register */
#define         HC_INT_PTD_LASTPTD_REG                  0x148   /* INT PTD Last PTD Register  */

/*ATL Transfer Registers */
#define         HC_ATL_PTD_DONEMAP_REG                  0x150   /* ATL PTD Last PTD Register  */
#define         HC_ATL_PTD_SKIPMAP_REG                  0x154   /* ATL PTD Last PTD Register  */
#define         HC_ATL_PTD_LASTPTD_REG                  0x158   /* ATL PTD Last PTD Register  */

/*General Purpose Registers */
#define         HC_HW_MODE_REG                          0x300   /* H/W Mode Register  */
#define         HC_CHIP_ID_REG                          0x304   /* Chip ID Register */
#define         HC_SCRATCH_REG                          0x308   /* Scratch Register */
#define         HC_RESET_REG                            0x30C   /* HC Reset Register */

/* Interrupt Registers */
#define         HC_INTERRUPT_REG                        0x310   /* Interrupt Register */
#define         HC_INTENABLE_REG                        0x314   /* Interrupt enable Register */
#define         HC_ISO_IRQ_MASK_OR_REG                  0x318   /* ISO Mask OR Register */
#define         HC_INT_IRQ_MASK_OR_REG                  0x31C   /* INT Mask OR Register*/
#define         HC_ATL_IRQ_MASK_OR_REG                  0x320   /* ATL Mask OR Register */
#define         HC_ISO_IRQ_MASK_AND_REG                 0x324   /* ISO Mask AND Register */
#define         HC_INT_IRQ_MASK_AND_REG                 0x328   /* INT Mask AND Register */
#define         HC_ATL_IRQ_MASK_AND_REG                 0x32C   /* ATL Mask AND Register */

/*RAM Registers */
#define         HC_DMACONFIG_REG                        0x330   /* DMA Config Register */
#define         HC_TRANS_COUNT_REG                      0x334   /* Transfer Counter Register */
#define         HC_BUFFER_STAT_REG                      0x338   /* Buffer status Register */
#define         HC_MEM_READ_REG                         0x33C   /* Memory Register */

/*interrupt count and buffer status register*/

#define         HC_BUFFER_STATUS_REG                    0x334
#define         HC_INT_THRESHOLD_REG                    0x340
#define         HC_OTG_INTERRUPT                        0x400

struct isp1761_driver;
typedef struct _isp1761_id {
    __u16 idVendor;
    __u16 idProduct;
    unsigned long driver_info;
}isp1761_id;

typedef struct isp1761_dev {
    /*added for pci device*/
    struct device *dev;
    struct  isp1761_driver *driver; /* which driver has allocated this device */
    void            *driver_data;   /* data private to the host controller driver */
    void            *otg_driver_data; /*data private for otg controler*/
    unsigned char   index;          /* local controller (HC/DC/OTG) */
    unsigned int    irq;    /*Interrupt Channel allocated for this device */
    void (*handler)(struct isp1761_dev *dev,
            void *isr_data);      /* Interrupt Serrvice Routine */
    void            *isr_data;              /* isr data of the driver */
    unsigned long   int_reg;                /* Interrupt register */
    unsigned long   alt_int_reg;            /* Interrupt register 2*/
    unsigned long   start;
    unsigned long   length;
    struct resource *mem_res;
    unsigned long   io_base;/* Start Io address space for this device */
    unsigned long   io_len; /* IO address space length for this device */

    unsigned long   chip_id; /* Chip Id */

    char            name[80];       /* device name */
    int             active;                         /* device status */

    /* DMA resources should come TODO */
    unsigned long   dma;
    u8 *baseaddress;  /*base address for i/o ops*/
    u8 *dmabase;
}isp1761_dev_t ;


typedef struct isp1761_driver {
    char            *name;
    unsigned long   index;  /* HC or DC or OTG */
    isp1761_id      *id;    /*device ids*/
    int (*probe)(struct isp1761_dev *dev,
            isp1761_id *id);        /* New device inserted */
    void (*remove)(struct isp1761_dev *dev);/* Device removed (NULL if not a hot-plug capable driver) */

#ifdef CONFIG_PM
    void (*suspend)(struct isp1761_dev *dev);       /* Device suspended */
    void (*resume)(struct isp1761_dev *dev);        /* Device woken up */
#endif /* CONFIG_PM */

} isp_1761_driver_t;

struct usb_device * phci_register_otg_device(struct isp1761_dev *dev);

/*otg exported function from host*/
int     phci_suspend_otg_port(struct isp1761_dev *dev, u32 command);
int     phci_enumerate_otg_port(struct isp1761_dev *dev, u32 command);

int     isp1761_register_driver(struct isp1761_driver *drv);
void    isp1761_unregister_driver(struct isp1761_driver *drv);
int     isp1761_request_irq(void(*handler)(struct isp1761_dev* dev,void* isr_data),
        struct isp1761_dev* dev, void* isr_data);
void    isp1761_free_irq(struct isp1761_dev* dev,void* isr_data);

__u32   isp1761_reg_read32(isp1761_dev_t *dev,__u16 reg,__u32 data);
void    isp1761_reg_write32(isp1761_dev_t *dev,__u16 reg,__u32 data);
int     isp1761_mem_read(isp1761_dev_t *dev, __u32 start_add, 
        __u32 end_add, __u32 * buffer, __u32 length, __u16 dir);
int     isp1761_mem_write(isp1761_dev_t *dev, __u32 start_add, 
        __u32 end_add, __u32 * buffer, __u32 length, __u16 dir);
#endif /* __HAL_INTF_H__ */
