/* i2ctest.c 
 ***********************************************************************
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Driver for testing i2c IP ocore with irq_mngr
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
 **********************************************************************
 */

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>	/* request_mem_region */
#include <linux/interrupt.h>
#include <linux/proc_fs.h>

#include <asm/io.h>	/* readw() writew() */
#include <mach/hardware.h>
#include <asm/irq.h>

/* Addresses */
#define FPGA_IRQ_MASK   (0x0)
#define FPGA_IRQ_PEND   (0x2)
#define FPGA_IRQ_ACK    (0x2)

#define FPGA_BASE_ADDR		(0x20)
#define FPGA_I2C_PRESCLo	(FPGA_BASE_ADDR+0x0)
#define FPGA_I2C_PRESCHi	(FPGA_BASE_ADDR+0x2)
#define FPGA_I2C_CTR		(FPGA_BASE_ADDR+0x4)
#define FPGA_I2C_TXR		(FPGA_BASE_ADDR+0xA)
#define FPGA_I2C_RXR		(FPGA_BASE_ADDR+0xE)
#define FPGA_I2C_CR		(FPGA_BASE_ADDR+0x8)
#define FPGA_I2C_SR		(FPGA_BASE_ADDR+0xC)

/* Values */
#define PRELo	(0xBF)
#define PREHi   (0x00)

/* read/write (accelerometer)*/
#define ADDR	(0x1D) /* lis3lv02dl */
#define SUBADDR (0x16) /* OFFSET_X   */
#define VALUE	(0xBB)

/* irq position in irqmngr */
#define IRQ (0x01)


/* CTR register */
#define I2C_EN    (0x80)
#define I2C_IEN   (0x40)

/* CR register */
#define I2C_STA   (0x80)
#define I2C_STO   (0x40)
#define I2C_RD    (0x20)
#define I2C_WR    (0x10)
#define I2C_ACK   (0x08)
#define I2C_IACK  (0x01)

/* read/write bit */
#define I2C_ADD_READ  (0x01)
#define I2C_ADD_WRITE (0x00)

//#define LED_DEBUG
#undef PDEBUG
#ifdef LED_DEBUG
# ifdef __KERNEL__
    /* for kernel spage */
#   define PDEBUG(fmt,args...) printk(KERN_DEBUG "i2ctest : " fmt, ##args)
# else
    /* for user space */
#   define PDEBUG(fmt,args...) printk(stderr, fmt, ##args)
# endif
#else
# define PDEBUG(fmt,args...) /* no debbuging message */
#endif

#define DRIVER_NAME "i2ctest"

struct i2ctest_dev {
	struct semaphore sem;
	void * fpga_virtual_base_address;
};

struct i2ctest_dev *i2cdev;

struct resource *mem; /* pointer to fpga led */

static void free_all(void);

/***************************
 * interrupt management
 * *************************/
static irqreturn_t fpga_interrupt(int irq,void *stuff,struct pt_regs *reg){
	static int count = 0;
	u16 data;

	/***************/
	/* write VALUE */
	/***************/
	if (count == 0) {
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);
		/* write to Transmit sub address, set WR bit acknowledge interrupt*/
		iowrite16(SUBADDR,i2cdev->fpga_virtual_base_address + FPGA_I2C_TXR);
		iowrite16(I2C_WR|I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;
	} else if (count == 1) {
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);

		/* write to Transmit sub address, set WR bit acknowledge interrupt*/
		iowrite16(ADDR<<1|I2C_ADD_WRITE,i2cdev->fpga_virtual_base_address+FPGA_I2C_TXR);
		iowrite16(I2C_STA|I2C_WR|I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count ++;
	} else if (count == 2) {
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);
		/* write VALUE, set WR bit acknowledge interrupt*/
		iowrite16(VALUE,i2cdev->fpga_virtual_base_address+FPGA_I2C_TXR);
		iowrite16(I2C_IACK|I2C_WR|I2C_STO,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;
	} else if (count == 3) {
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);
		/* acknowledge interruption */
		iowrite16(I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);
		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;

	} else if (count == 4) {/**************READ THE VALUE ****************/
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);

		/* write 0x2C to Transmit sub address, set WR bit acknowledge interrupt*/
		iowrite16(SUBADDR,i2cdev->fpga_virtual_base_address+FPGA_I2C_TXR);
		iowrite16(I2C_WR|I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;
	} else if (count == 5) {
		/*read RxACK bit from Status Register, should be 0 */
		ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);
		/* write adress on TXR and CTR register repeated start, acknowledge interrupt*/
		iowrite16(ADDR<<1|I2C_ADD_READ,i2cdev->fpga_virtual_base_address+FPGA_I2C_TXR);
		iowrite16(I2C_STA|I2C_WR|I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;
	} else if (count == 6) {
		/* write on TXR and CTR register, set read bit,acknowledge interrupt*/
		iowrite16(I2C_RD|I2C_ACK|I2C_IACK|I2C_STO,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
		count++;
	} else if (count == 7) {
		/*read RxACK bit from Status Register, should be 0 */
		data = ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_SR);
		/* read the value */
		data = ioread16(i2cdev->fpga_virtual_base_address + FPGA_I2C_RXR);
		/* write on TXR and CTR register, set read bit,acknowledge interrupt*/
		iowrite16(I2C_IACK,
				  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);

		/* acknowledge irq in irq manager*/
		iowrite16(IRQ,i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);

		printk(KERN_INFO "read %04x\n",data);
		count++;
	}
	return IRQ_HANDLED;
}

/****************************
 * read proc
 * **************************/

static int read_proc(char *page,char **start,off_t offset,
					 int count,int *eof,void *data)
{
	PDEBUG("Trying to write %04x on i2c slave\n",VALUE);

	/* unmask interrupt for i2c in interrupt manager*/
	iowrite16(IRQ, i2cdev->fpga_virtual_base_address + FPGA_IRQ_MASK);

	/* set prescale */
	iowrite16(PRELo, i2cdev->fpga_virtual_base_address + FPGA_I2C_PRESCLo);
	PDEBUG("Write %04x at %04x\n", PRELo,FPGA_I2C_PRESCLo);
	iowrite16(PREHi, i2cdev->fpga_virtual_base_address + FPGA_I2C_PRESCHi);
	PDEBUG("Write %04x at %04x\n", PRELo,FPGA_I2C_PRESCHi);

	/* set core enable and interrupt enable*/
	iowrite16(I2C_EN|I2C_IEN,
			  i2cdev->fpga_virtual_base_address + FPGA_I2C_CTR);
	PDEBUG("Write %04x at %04x\n", I2C_EN|I2C_IEN,FPGA_I2C_CTR);

	/* write slave adresse on TXR */
	iowrite16(ADDR<<1|I2C_ADD_WRITE, i2cdev->fpga_virtual_base_address + FPGA_I2C_TXR);
	/* and CTR register*/
	iowrite16(I2C_STA|I2C_WR|I2C_IACK,
			  i2cdev->fpga_virtual_base_address+FPGA_I2C_CR);
	PDEBUG("Write %04x at %04x\n", I2C_STA|I2C_WR,FPGA_I2C_TXR);

	return 0;
}

static int read_proc2(char *page,char **start,off_t offset,int count,
					  int *eof,void *data)
{
	PDEBUG("Trying to read on i2c slave\n");

	/* write slave adresse on TXR*/
	iowrite16(ADDR<<1|I2C_ADD_WRITE, i2cdev->fpga_virtual_base_address + FPGA_I2C_TXR);
	/* write ctr register*/
	iowrite16(I2C_ADD_WRITE|I2C_STA|I2C_WR|I2C_IACK,
			  i2cdev->fpga_virtual_base_address + FPGA_I2C_CR);
	return 0;
}

/****************************
 * Module management 
 * **************************/

static int __init i2ctest_init(void){
	int result;
	u16 data;
	result = 0;

	mem = NULL;

	/* Allocate a private structure and reference it as driver's data */
	PDEBUG("Allocate a private structure and reference it as driver s data\n"); 
	i2cdev = (struct i2ctest_dev *)kmalloc(sizeof(struct i2ctest_dev),GFP_KERNEL);
	if (i2cdev == NULL) {
		printk(KERN_WARNING "I2CDEV: unable to allocate private structure\n");
		return -ENOMEM;
	}

	/* Requested I/O memory */
	if ((i2cdev->fpga_virtual_base_address = ioremap(IMX_CS1_PHYS, IMX_CS1_SIZE)) == NULL) {
		printk(KERN_ERR "ioremap error\n");
		goto error;
	}

	/* irq registering */
	if (request_irq(IRQ_GPIOA(1), (irq_handler_t)fpga_interrupt, IRQF_SHARED,"fpga", i2cdev) < 0) {
		printk(KERN_ERR "Can't request fpga irq\n");
		goto error;
	}
	//ioread16(i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);
	/* irq ack */
	data = IRQ;
	iowrite16(data, i2cdev->fpga_virtual_base_address + FPGA_IRQ_ACK);

	/* create proc entry */
	create_proc_read_entry("i2c_test",0 /* default mode */,
						   NULL /* parent dir */, read_proc, 
						   NULL /* client data */);

	/* create proc entry */
	create_proc_read_entry("i2c_test2",0 /* default mode */,
						   NULL /* parent dir */, read_proc2, 
						   NULL /* client data */);

	/* OK module inserted ! */
	printk(KERN_INFO "i2ctest module insered\n");
	return 0;

error:
	printk(KERN_ERR "%s: not inserted\n",DRIVER_NAME);
	free_all();
	return result;


}

static void __exit i2ctest_exit(void){
	free_all();
}

static void free_all(void){

	/* free irq*/
	free_irq(IRQ_GPIOA(1),i2cdev);

	/* Free the allocated memory */
	kfree(i2cdev);

	/* remove proc entry */
	remove_proc_entry("i2c_test",0);

	printk(KERN_INFO "i2ctest module deleted\n");
}

module_init(i2ctest_init);
module_exit(i2ctest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("I2C tester");

