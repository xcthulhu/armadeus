/************************************************************
 * Philips ISP176x Hardware Abstraction Layer code file
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
 * File Name: hal_x86.c
 *
 * Refering linux kernel version 2.6.23
 *
 * History:
 *
 * Date                Author                  Comments
 * ---------------------------------------------------------------------
 * Nov 29 2005        Prabhakar Kalasani      Initial Creation     
 * Nov 04 2007        NC (armadeus)           add 2.6.23 compatibility
 **********************************************************************
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>  
#include <linux/usb.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <asm/dma.h>

/*--------------------------------------------------------------*
 *               linux system include files
 *--------------------------------------------------------------*/
#include "hal_x86.h" // We can use the x86 headers for now.
#include "../hal/hal_intf.h"
#include "../hal/isp1761.h"

/*--------------------------------------------------------------*
 *               Local variable Definitions
 *--------------------------------------------------------------*/
struct isp1761_dev              isp1761_loc_dev[ISP1761_LAST_DEV];
static  struct isp1761_hal      hal_data;
static struct platform_device *s_pdev;
int      iolength = 0;
static   __u32            isp1761_base = 0;

/*--------------------------------------------------------------*
 *               Local # Definitions
 *--------------------------------------------------------------*/
#define         isp1761_driver_name     "1761-m6"
#define         ISP1761_IO_EXTENT ((1 << 17)-1) // 17 address bits used.
#define         ISP1761_REGSET_LEN   0xffff

#undef CONFIG_PM /* PM no workie yet. */

/*--------------------------------------------------------------*
 *               Local Function 
 *--------------------------------------------------------------*/

static int __devexit isp1761_remove (struct platform_device *pdev);
static int __devinit isp1761_probe (struct platform_device *pdev);
#ifdef CONFIG_PM
static int isp1761_pci_suspend (struct pci_dev *dev, __u32 state);
static int isp1761_pci_resume (struct pci_dev *dev);
#endif
static irqreturn_t  isp1761_pci_isr (int irq, void *dev_id);


/*--------------------------------------------------------------*
 *              ISP 1761 interrupt locking functions
 *--------------------------------------------------------------*/
int             isp1761_hw_lock = 0;
int             isp1761_hw_isr = 0;

void isp1761_disable_interrupt(int      irq) {
    /* DUMMY functions
     * Not used */
    disable_irq(irq);
    return;
}
void isp1761_enable_interrupt(int       irq) {
    /* DUMMY functions
     * Not used */
    enable_irq(irq);
    return;
}

/*--------------------------------------------------------------*
 *               ISP1761 Interrupt Service Routine
 *--------------------------------------------------------------*/
/*Interrupt Service Routine for device controller*/
irqreturn_t isp1761_pci_dc_isr(int irq, void *data)
{
    struct isp1761_dev *dev;
    dev = &isp1761_loc_dev[ISP1761_DC];

    hal_entry("%s: Entered\n",__FUNCTION__);
    /*not ready yet*/
    if(dev->active == 0)
    {
        printk("isp1761_pci_dc_isr: dev->active is NULL \n");
        return IRQ_NONE;
    }

    /* Get the source of interrupts for Device Controller
     * Device Controller interrupts are cleared by the driver
     * during processing
     */

    /*unblock the device interrupt*/

    isp1761_reg_write32(dev, DEV_UNLOCK_REGISTER, 0xaa37);
    dev->int_reg = isp1761_reg_read32(dev, DEV_INTERRUPT_REGISTER, dev->int_reg);
    hal_int("isp1761_pci_dc_isr:INTERRUPT_REGISTER 0x%x\n",dev->int_reg);
    /*clear the interrupt source*/
    isp1761_reg_write32(dev, 0x218, dev->int_reg);
    dev->int_reg &= 0x03fffdb9;
    if(dev->int_reg)
        dev->handler(dev, dev->isr_data);
    hal_entry("%s: Exit\n",__FUNCTION__);
    return IRQ_HANDLED;
}

/* Interrupt Service Routine of isp1761                                   
 * Reads the source of interrupt and calls the corresponding driver's ISR.
 * Before calling the driver's ISR clears the source of interrupt.
 * The drivers can get the source of interrupt from the dev->int_reg field
 */
irqreturn_t isp1761_pci_isr(int irq, void *__data) 
{
    __u32               irq_mask = 0;
    struct isp1761_dev  *dev;
    hal_entry("%s: Entered\n",__FUNCTION__);
    /* Process the Host Controller Driver */
    dev = &isp1761_loc_dev[ISP1761_HC];
    /* Get the source of interrupts for Host Controller*/
    dev->int_reg = isp1761_reg_read32(dev, HC_INTERRUPT_REG,dev->int_reg);
    isp1761_reg_write32(dev,HC_INTERRUPT_REG,dev->int_reg);
    irq_mask = isp1761_reg_read32(dev, HC_INTENABLE_REG,irq_mask);

    dev->int_reg &= irq_mask; /*shared irq ??*/
    /*call the Host Isr if any valid(minus otg)interrupt is present*/
    if(dev->int_reg & ~HC_OTG_INTERRUPT)                
        dev->handler(dev,dev->isr_data);
#ifdef OTG
#ifndef MSEC_INT_BASED
    mdelay(1);
#endif
    /*process otg interrupt if there is any*/
    if(dev->int_reg & HC_OTG_INTERRUPT){
        u32     otg_int;        
#ifndef MSEC_INT_BASED
        mdelay(1);
#endif
        otg_int = (dev->int_reg & HC_OTG_INTERRUPT);
        /* Process OTG controller Driver 
         * Since OTG is part of  HC interrupt register, 
         * the interrupt source will be HC interrupt Register
         * */
        dev = &isp1761_loc_dev[ISP1761_OTG];
        /* Read the source of  OTG_INT and clear the
           interrupt source */
        dev->int_reg = otg_int; 
        dev->handler(dev, dev->isr_data);
    }   
#endif
    hal_entry("%s: Exit\n",__FUNCTION__);
    return IRQ_HANDLED;
} /* End of isp1362_pci_isr */

/*--------------------------------------------------------------*
 *               PCI Driver Interface Functions
 *--------------------------------------------------------------*/

/* Pci driver interface functions */
static struct platform_driver isp1761_platform_driver = {
probe:         isp1761_probe,
remove:        isp1761_remove,
driver:        { .name = "isp1761" }
};


/*--------------------------------------------------------------*
 *               ISP1761 Read write routine 
 *--------------------------------------------------------------*/

/* Write a 32 bit Register of isp1761 */
void isp1761_reg_write32(struct isp1761_dev *dev,__u16 reg,__u32 data)
{ 
    /* Write the 32bit to the register address given to us*/
    writel(data,dev->baseaddress+reg);
    //printk("Wrote to 0x%08x value 0x%08x\n", isp1761_base + reg, data);
}


/* Read a 32 bit Register of isp1761 */
__u32 isp1761_reg_read32(struct isp1761_dev *dev,__u16 reg,__u32 data)
{ 

    data = readl(dev->baseaddress + reg);
    //printk("Read from 0x%08x is 0x%08x\n", isp1761_base + reg, data);
    return data;
}


/* Read a 16 bit Register of isp1761 */
__u16 isp1761_reg_read16(struct isp1761_dev *dev,__u16 reg,__u16 data)
{ 
    data = readw(dev->baseaddress+reg);
    return data;
}

/* Write a 16 bit Register of isp1761 */
void isp1761_reg_write16(struct isp1761_dev *dev,__u16 reg,__u16 data)
{ 
    //printk("Wrote to 0x%08x value 0x%04x\n", isp1761_base + reg, data);
    writew(data,dev->baseaddress+reg);

}

/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_mem_read
 *
 * Memory read using PIO method.
 *
 *  Input: struct isp1761_driver *drv  -->  Driver structure.
 *                      __u32 start_add     --> Starting address of memory 
 *              __u32 end_add     ---> End address 
 *              
 *              __u32 * buffer      --> Buffer pointer.
 *              __u32 length       ---> Length 
 *              __u16 dir          ---> Direction ( Inc or Dec)
 *                      
 *  Output     int Length  ----> Number of bytes read 
 *
 *  Called by: system function 
 * 
 * 
 *--------------------------------------------------------------*/
/* Memory read function PIO */

int     
isp1761_mem_read(struct isp1761_dev *dev, __u32 start_add, 
        __u32 end_add, __u32 * buffer, __u32 length, __u16 dir)
{
    u8 *temp_base_mem = 0;      
    u8 *one = (u8 *) buffer;
    u16 *two = (u16 *) buffer;
    int a = (int)length;
    u32 w;
    temp_base_mem= (dev->baseaddress + start_add);
    /*initialize the Register 0x33C-used to manage Multiple threads */
    writel(start_add,dev->baseaddress+0x33c);

last:
    w = readl(temp_base_mem);
    if(a == 1){
        *one=(u8)w;
        return 0;
    }
    if(a == 2){
        *two=(u16)w;
        return 0;
    }   


    if(a == 3){
        *two=(u16)w;
        two += 1;
        w >>= 16;
        *two = (u8)(w);
        return 0;

    }


    while(a>0){
        *buffer = w;
        temp_base_mem = temp_base_mem+4;
        start_add +=4;
        a -= 4;
        if(a == 0)
            break;
        if(a < 4){
            buffer += 1;
            one = (u8 *)buffer;
            two = (u16 *)buffer;
            goto last;
        }
        buffer += 1;
        w = readl(temp_base_mem); 
    }
    return ((a < 0) || (a == 0))?0:(-1);

}

/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_mem_write
 *
 * Memory write using PIO method.
 *
 *  Input: struct isp1761_driver *drv  -->  Driver structure.
 *                      __u32 start_add     --> Starting address of memory 
 *              __u32 end_add     ---> End address 
 *              
 *              __u32 * buffer      --> Buffer pointer.
 *              __u32 length       ---> Length 
 *              __u16 dir          ---> Direction ( Inc or Dec)
 *                      
 *  Output     int Length  ----> Number of bytes read 
 *
 *  Called by: system function 
 * 
 * 
 *--------------------------------------------------------------*/

/* Memory read function IO */
int     
isp1761_mem_write(struct isp1761_dev *dev, 
        __u32 start_add, __u32 end_add, 
        __u32 * buffer, __u32 length,
        __u16 dir)
{
    u8 *temp_base_mem = 0;
    int a = length;
    //u8 *temp = (u8*)buffer;
    u8 one      =(u8 )(*buffer);
    u16 two     =(u16 )(*buffer);       
    temp_base_mem = (dev->baseaddress + start_add);

    if(a == 1){
        writeb(one,temp_base_mem);
        return 0;
    }
    if(a == 2){
        writew(two,temp_base_mem);
        return 0;
    }

    while(a>0){         
        writel(*buffer, temp_base_mem);
        temp_base_mem = temp_base_mem+4;
        start_add +=4;
        a -=4;
        if(a <= 0)
            break;
        buffer += 1;

    }
    return ((a < 0) || (a == 0))?0:(-1);

}
/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_check_mem_region
 *
 *  Check the memory region for Memory Mapping 
 *  Check with the system about the availability of the region,
 *  and returns success, if available.
 *
 *  Input: struct isp1761_driver *drv  --> Driver structure.
 *  
 *  Output result  
 *         
 *
 *  Called by: system function 
 * 
 * 
 *--------------------------------------------------------------*/

int isp1761_check_mem_region(struct isp1761_dev *dev)
{
    int ret;
    ret=check_mem_region(dev->start, dev->length);
    return ret;
}/* End of isp1761_check_mem_region */

/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_request_mem_region
 isp1761_release_mem_region
 isp1761_get_mem_params

 *
 *  If the check returns Success, we can request the region for 
 *  Memory mapping of our chip memory
 *
 *  Input: struct isp1761_driver *drv  --> Driver structure.
 *  
 *  Output result  
 *         
 *
 *  Called by: system function 
 * 
 * 
 *--------------------------------------------------------------*/

struct resource* isp1761_request_mem_region(struct isp1761_dev *dev)
{
    dev->mem_res = request_mem_region(dev->start, dev->length, "Isp1761_device");
    return dev->mem_res;
}/* End of isp1761_request_mem_region */

/* Release an already acquired memory region.
   It should be done at the rmmod of the module */
void isp1761_release_mem_region(struct isp1761_dev* dev)
{
    release_mem_region (dev->start, dev->length);
}

/* Get the start address and length of Mapped Memory */ 
void isp1761_get_mem_params(struct isp1761_dev *dev,struct isp1761_driver *drv)
{
    dev->start  =isp1761_loc_dev[drv->index].start;
    dev->length =isp1761_loc_dev[drv->index].length;
}/* End of isp1761_get_mem_params*/


/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_request_irq
 *
 * This function registers the ISR of driver with this driver.
 * Since there is only one interrupt line, when the first driver
 * is registerd, will call the system function request_irq. The PLX
 * bridge needs enabling of interrupt in the interrupt control register to 
 * pass the local interrupts to the PCI (cpu).
 * For later registrations will just update the variables. On ISR, this driver
 * will look for registered handlers and calls the corresponding driver's
 * ISR "handler" function with "isr_data" as parameter.
 *
 *  Input: struct 
 *              (void (*handler)(struct isp1761_dev *, void *)-->handler.
 *               isp1761_driver *drv  --> Driver structure.
 *  Output result 
 *         0= complete 
 *         1= error.
 *
 *  Called by: system function module_init 
 * 
 * 
 *--------------------------------------------------------------*/

int isp1761_request_irq(void (*handler)(struct isp1761_dev *, void *),
        struct isp1761_dev *dev, void *isr_data) 
{
    int result = 0;
    hal_entry("%s: Entered\n",__FUNCTION__);
    hal_int("isp1761_request_irq: dev->index %x\n",dev->index);
    if(dev->index == ISP1761_DC){
        result = request_irq(dev->irq, isp1761_pci_dc_isr,
                IRQF_SHARED,
                dev->name,
                isr_data);
    }else {
        result= request_irq(dev->irq,isp1761_pci_isr,
                IRQF_SHARED,
                dev->name,
                isr_data);
    }

    /*Interrupt handler routine*/
    dev->handler = handler;
    dev->isr_data = isr_data;
    hal_int("isp1761_request_irq: dev->handler %s\n",dev->handler);
    hal_int("isp1761_request_irq: dev->isr_data %x\n",dev->isr_data);
    hal_entry("%s: Exit\n",__FUNCTION__);
    return result;
} /* End of isp1761_request_irq */

/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_free_irq
 *
 * This function de-registers the ISR of driver with this driver.
 * Since there is only one interrupt line, when the last driver
 * is de-registerd, will call the system function free_irq. The PLX
 * bridge needs disabling of interrupt in the interrupt control register to 
 * block the local interrupts to the PCI (cpu).
 *
 *  Input: struct 
 *              (void (*handler)(struct isp1761_dev *, void *)-->handler.
 *               isp1761_driver *drv  --> Driver structure.
 *  Output result 
 *         0= complete 
 *         1= error.
 *
 *  Called by: system function module_init 
 * 
 * 
 *--------------------------------------------------------------*/

void isp1761_free_irq(struct isp1761_dev *dev, void *isr_data)
{
    hal_int(("isp1761_free_irq(dev=%p,isr_data=%p)\n",dev,isr_data));
    free_irq(dev->irq,isr_data);
} /* isp1761_free_irq */



/* Allocate Fragmented kernel Memory */
void* isp_1761_kmalloc(size_t size,int flags)
{
    void* ret;
    ret =kmalloc(size,flags);
    return ret;
}

/* Free the memory allocated by kmalloc */
void isp_1761_kfree(const void* objp)
{
    kfree(objp);
}

/* Allocate Contiguous kernel Memory */
void* isp_1761_vmalloc(__u32 size, __u16 flags, pgprot_t prot)
{
    void* ret;
    ret = __vmalloc(size, flags, prot);
    return ret;
}

/* Free the memory allocated by vmalloc */
void isp_1761_vfree(const void* objp)
{
    kfree(objp);
}



/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_register_driver
 *
 * This function is used by top driver (OTG, HCD, DCD) to register
 * their communication functions (probe, remove, suspend, resume) using
 * the drv data structure.
 * This function will call the probe function of the driver if the ISP1761
 * corresponding to the driver is enabled
 *
 *  Input: struct isp1761_driver *drv  --> Driver structure.
 *  Output result 
 *         0= complete 
 *         1= error.
 *
 *  Called by: system function module_init 
 * 
 * 
 *--------------------------------------------------------------*/

int     isp1761_register_driver(struct isp1761_driver *drv) 
{
    struct isp1761_dev  *dev;
    int result;
    isp1761_id *id;

    hal_entry("%s: Entered\n",__FUNCTION__);
    info("isp1761_register_driver(drv=%p) \n",drv);

    if(!drv) return -EINVAL;
    dev = &isp1761_loc_dev[drv->index];
    if(drv->index == ISP1761_DC){/*FIX for device*/
        result = drv->probe(dev,drv->id);
    }else{              
        id = drv->id;
        if(dev->active) result = drv->probe(dev,id);
        else    result = -ENODEV;
    }

    if(result >= 0 ) {
        printk(KERN_INFO __FILE__ ": Registered Driver %s\n",
                drv->name);
        dev->driver = drv;
    }
    hal_entry("%s: Exit\n",__FUNCTION__);
    return result;
} /* End of isp1761_register_driver */


/*--------------------------------------------------------------*
 *  
 * Module dtatils: isp1761_unregister_driver
 *
 * This function is used by top driver (OTG, HCD, DCD) to de-register
 * their communication functions (probe, remove, suspend, resume) using
 * the drv data structure.
 * This function will check whether the driver is registered or not and
 * call the remove function of the driver if registered
 *
 *  Input: struct isp1761_driver *drv  --> Driver structure.
 *  Output result 
 *         0= complete 
 *         1= error.
 *
 *  Called by: system function module_init 
 * 
 * 
 *--------------------------------------------------------------*/


void    isp1761_unregister_driver(struct isp1761_driver *drv)
{
    struct isp1761_dev  *dev;
    hal_entry("%s: Entered\n",__FUNCTION__);

    info("isp1761_unregister_driver(drv=%p)\n",drv);
    dev = &isp1761_loc_dev[drv->index];
    if(dev->driver == drv) {
        /* driver registered is same as the requestig driver */
        drv->remove(dev);
        dev->driver = NULL;
        info(": De-registered Driver %s\n",
                drv->name);
        return;
    }
    hal_entry("%s: Exit\n",__FUNCTION__);
} /* End of isp1761_unregister_driver */


/*--------------------------------------------------------------*
 *               ISP1761 PCI driver interface routine.
 *--------------------------------------------------------------*/


/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_module_init
 *
 *  This  is the module initialization function. It registers to 
 *  PCI driver for a PLX PCI bridge device. And also resets the
 *  internal data structures before registering to PCI driver.
 *
 *  Input: void 
 *  Output result 
 *         0= complete 
 *         1= error.
 *
 *  Called by: system function module_init 
 * 
 * 
 * 
 -------------------------------------------------------------------*/
static int __init isp1761_module_init (void) 
{
    int result = 0;
    hal_entry("%s: Entered\n",__FUNCTION__);

    memset(isp1761_loc_dev,0,sizeof(isp1761_loc_dev));

    result = platform_driver_register(&isp1761_platform_driver);

    hal_entry("%s: Exit\n",__FUNCTION__);
    return result;
}

/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_module_cleanup
 *
 * This  is the module cleanup function. It de-registers from 
 * PCI driver and resets the internal data structures.
 *
 *  Input: void 
 *  Output void
 *
 *  Called by: system function module_cleanup 
 * 
 * 
 * 
 --------------------------------------------------------------*/

static void __exit isp1761_module_cleanup (void) 
{
    printk("Hal Module Cleanup\n");
    platform_driver_unregister(&isp1761_platform_driver);
    memset(isp1761_loc_dev,0,sizeof(isp1761_loc_dev));
} 


/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_probe
 *
 * PCI probe function of ISP1761
 * This function is called from PCI Driver as an initialization function
 * when it founds the PCI device. This functions initializes the information
 * for the 3 Controllers with the assigned resources and tests the register
 * access to these controllers and do a software reset and makes them ready
 * for the drivers to play with them.
 *
 *  Input: 
 *              struct pci_dev *dev                     ----> PCI Devie data structure 
 *      const struct pci_device_id *id  ----> PCI Device ID 
 *  Output void
 *
 *  Called by: system function module_cleanup 
 * 
 * 
 * 
 --------------------------------------------------------------**/

    static int __devinit
isp1761_probe (struct platform_device *pdev)
{
    __u32       reg_data = 0;
    struct isp1761_dev  *loc_dev;
    struct resource *res;
    void *address = 0;
    int length = ISP1761_IO_EXTENT;
    int status = 0;
    hal_entry("%s: Entered\n",__FUNCTION__);

    res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "isp1761-regs");
    if (!res)
	    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res)
	    return -ENODEV;

    isp1761_base = res->start;
    hal_init(("isp1761 pci base address = %x\n", isp1761_base));

    /* Get the Host Controller IO and INT resources
     */
    loc_dev = &(isp1761_loc_dev[ISP1761_HC]);
    loc_dev->dev = &pdev->dev;
    loc_dev->irq = platform_get_irq(pdev, 0);
    loc_dev->io_base = isp1761_base;
    loc_dev->start   =  isp1761_base;
    loc_dev->length  = ISP1761_REGSET_LEN;
    loc_dev->io_len = ISP1761_REGSET_LEN; /*64K*/
    loc_dev->index = ISP1761_HC;/*zero*/

    loc_dev->io_len = length; 
    if(check_mem_region(loc_dev->io_base,length)<0){
        err("host controller already in use\n");
        return -EBUSY;
    }
    if(!request_mem_region(loc_dev->io_base, length,isp1761_driver_name)){
        err("host controller already in use\n");
        return -EBUSY;

    }

    /*map available memory*/
    address = ioremap_nocache(isp1761_base,length);
    if(address == NULL){
        err("memory map problem\n");
        release_mem_region(loc_dev->io_base,length);
        return -ENOMEM;
    } 

    loc_dev->baseaddress = (u8*)address;
    //loc_dev->dmabase = (u8*)iobase;

    hal_init(("isp1761 HC MEM Base= %p irq = %d\n", 
                loc_dev->baseaddress,loc_dev->irq));
#ifdef ISP1761_DEVICE   

    /*initialize device controller framework*/  
    loc_dev = &(isp1761_loc_dev[ISP1761_DC]);
    loc_dev->dev = &pdev->dev;
    loc_dev->irq = platform_get_irq(pdev, 0);
    loc_dev->io_base = isp1761_base;
    loc_dev->start   = isp1761_base;
    loc_dev->length  = ISP1761_REGSET_LEN;
    loc_dev->io_len = ISP1761_REGSET_LEN;
    loc_dev->index = ISP1761_DC;
    loc_dev->baseaddress = address;
    loc_dev->active = 1;
    memcpy(loc_dev->name,"isp1761_dev",11);
    loc_dev->name[12] = '\0';


    {
        u32 chipid = 0;
        chipid = readl(address + 0x270);
        info("pid %04x, vid %04x\n", (chipid & 0xffff), (chipid >> 16));
    }   
    hal_init(("isp1761 DC MEM Base= %lx irq = %d\n", 
                loc_dev->io_base,loc_dev->irq));
    /* Get the OTG Controller IO and INT resources
     * OTG controller resources are same as Host Controller resources
     */
    loc_dev = &(isp1761_loc_dev[ISP1761_OTG]);
    loc_dev->dev = &pdev->dev;
    loc_dev->irq = platform_get_irq(pdev, 0);
    loc_dev->io_base = isp1761_base;
    loc_dev->start   =  isp1761_base;
    loc_dev->length  = ISP1761_REGSET_LEN;     
    loc_dev->io_len = ISP1761_REGSET_LEN;
    loc_dev->index = ISP1761_OTG; 
    loc_dev->baseaddress = address; /*having the same address as of host*/
    loc_dev->active = 1;
    memcpy(loc_dev->name,"isp1761_otg",11);
    loc_dev->name[12] = '\0';

    hal_init(("isp1761 OTG MEM Base= %lx irq = %x\n", 
                loc_dev->io_base,loc_dev->irq));

#endif

    /* Try to check whether we can access Scratch Register of
     * Host Controller or not.
     */

#if 1 // CUSTOM STUFF - 16-bit mode
    isp1761_reg_write32(loc_dev, HC_HW_MODE_REG, 0x82);
#endif

    loc_dev = &(isp1761_loc_dev[ISP1761_HC]);
    isp1761_reg_write32(loc_dev, HC_SCRATCH_REG, 0xFACEECAF);
    /* perform a read of the chip ID to avoir bus hold effect when
    * reading back the scratch register */
    isp1761_reg_read16(loc_dev, HC_CHIP_ID_REG, reg_data);
    udelay(100); 
    reg_data = isp1761_reg_read32(loc_dev, HC_SCRATCH_REG,reg_data);

    /* Host Controller presence is detected by writing to scratch register
     * and reading back and checking the contents are same or not
     */
    if(reg_data != 0xFACEECAF) {
        err("%s scratch register mismatch %x",
                isp1761_driver_name,reg_data);
        err("ISP176x device not present");
        status = -ENODEV;
        goto clean;
    }

    memcpy(loc_dev->name, isp1761_driver_name, sizeof(isp1761_driver_name));
    loc_dev->name[sizeof(isp1761_driver_name)] = 0;
    loc_dev->active = 1;

    info("controller address %p\n", &pdev->dev);

    s_pdev = pdev;
    hal_data.irq_usage = 0;
    hal_entry("%s: Exit\n",__FUNCTION__);
    return 0;

clean:
    release_mem_region(loc_dev->io_base, loc_dev->io_len);
    iounmap(loc_dev->baseaddress);
    hal_entry("%s: Exit\n",__FUNCTION__);
    return status;
} /* End of isp1761_probe */


/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_remove
 *
 * PCI cleanup function of ISP1761
 * This function is called from PCI Driver as an removal function
 * in the absence of PCI device or a de-registration of driver.
 * This functions checks the registerd drivers (HCD, DCD, OTG) and calls
 * the corresponding removal functions. Also initializes the local variables
 * to zero.
 *
 *  Input: 
 *              struct pci_dev *dev                     ----> PCI Devie data structure 
 *    
 *  Output void
 *
 *  Called by: system function module_cleanup 
 * 
 * 
 * 
 --------------------------------------------------------------*/
static int __devexit
isp1761_remove (struct platform_device *pdev)
{
    struct isp1761_dev  *loc_dev;
    hal_init(("isp1761_remove(dev=%p)\n",pdev));
    /*Lets handle the host first*/
    loc_dev  = &isp1761_loc_dev[ISP1761_HC];
    /*free the memory occupied by host*/
    release_mem_region(loc_dev->io_base, loc_dev->io_len);      
    /*unmap the occupied memory resources*/
    iounmap(loc_dev->baseaddress);
    s_pdev = NULL;
    return 0;
} /* End of isp1761_remove */

/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_pci_suspend
 *
 * PCI suspend function of ISP1761
 * This function is called from PCI Driver.
 * This functions checks the registerd drivers (HCD, DCD, OTG) and calls
 * the corresponding suspend functions if present. 
 *  Input: 
 *              struct pci_dev *dev                     ----> PCI Devie data structure 
 *    
 *  Output void
 *
 *  Called by: system function 
 * 
 * 
 * 
 --------------------------------------------------------------*/

#ifdef CONFIG_PM
static int isp1761_pci_suspend (struct pci_dev *dev, __u32 state) 
{
    struct isp1761_dev  *loc_dev;
    int                 index;

    hal_init(("isp1761_pci_suspend(dev=%p, state = %x)\n",dev, state));


    loc_dev = (struct isp1761_dev *)pci_get_drvdata(dev);

    /* For each controller check whether driver is registerd
     * or not. If registerd call the suspend function if it is
     * present
     */
    for(index=ISP1761_1ST_DEV;index<ISP1761_LAST_DEV;(index++,loc_dev++)) {
        if(loc_dev->driver && loc_dev->driver->suspend) {
            loc_dev->driver->suspend(loc_dev);
        }
    }

    return 0;
} /* End of isp1761_pci_suspend */
#endif


/*--------------------------------------------------------------*
 *
 *  Module dtatils: isp1761_pci_suspend
 *
 *  PCI resume function of ISP1761
 * This function is called from PCI Driver.
 * This functions checks the registerd drivers (HCD, DCD, OTG) and calls
 * the corresponding resume functions if present.  
 *  Input: 
 *              struct pci_dev *dev                     ----> PCI Devie data structure 
 *    
 *  Output void
 *
 *  Called by: system function
 * 
 * 
 --------------------------------------------------------------*/
#ifdef CONFIG_PM
static int isp1761_pci_resume (struct pci_dev *dev)
{
    struct isp1761_dev  *loc_dev;
    int                 index;
    hal_init(("isp1362_pci_resume(dev=%p)\n",dev));
    loc_dev = (struct isp1761_dev *)pci_get_drvdata(dev);

    /* For each controller check whether driver is registerd
     * or not. If registerd call the resume function if it is
     * present
     */
    for(index=ISP1761_1ST_DEV;index<ISP1761_LAST_DEV;(index++,loc_dev++)) {
        if(loc_dev->driver && loc_dev->driver->resume) {
            loc_dev->driver->resume(loc_dev);
        }
    }

    return 0;

} /* End of isp1362_pci_resume */
#endif



EXPORT_SYMBOL(isp1761_reg_write16);
EXPORT_SYMBOL(isp1761_reg_read16);
EXPORT_SYMBOL(isp1761_reg_read32);
EXPORT_SYMBOL(isp1761_reg_write32);
EXPORT_SYMBOL(isp1761_request_irq);
EXPORT_SYMBOL(isp1761_mem_read);
EXPORT_SYMBOL(isp1761_mem_write);
EXPORT_SYMBOL(isp1761_free_irq);
EXPORT_SYMBOL(isp1761_register_driver);
EXPORT_SYMBOL(isp1761_unregister_driver);

MODULE_AUTHOR (DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE ("GPL");

module_init (isp1761_module_init);
module_exit (isp1761_module_cleanup);


