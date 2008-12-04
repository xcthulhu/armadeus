/* i2ctest.h
 ***********************************************************************
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * module for testing i2c_master fpga IP
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
 --------------------------------------------
 *  18 mars 2008
 *  initial version
 */

#ifndef __I2CTEST_H_
#define __I2CTEST_H_

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#include <linux/config.h>
#endif

/* for module */
#include <linux/init.h>
#include <linux/module.h>

/* proc entry */
#include <linux/proc_fs.h>

/* request_mem_region */
#include <linux/ioport.h>

/* readw() writew() */
#include <asm/io.h>

/* hardware addresses */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#include <asm/hardware.h>
#else
#include <mach/hardware.h>
#endif

/* interruptions */
#include <linux/interrupt.h>
#include <asm/irq.h>

/* /proc */
#include <linux/proc_fs.h>

#endif

/* for debugging messages*/
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
#define FPGA_I2C_CR			(FPGA_BASE_ADDR+0x8)
#define FPGA_I2C_SR			(FPGA_BASE_ADDR+0xC)

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

#define DRIVER_NAME "i2ctest"

struct i2ctest_dev {
  struct semaphore sem;
  void * fpga_virtual_base_address;
};

struct resource *mem; /* pointer to fpga led */

struct i2ctest_dev *i2cdev;

static int __init i2ctest_init(void);
static void __exit i2ctest_exit(void);

static int read_proc(char *page, char **start, off_t offset,
					 int count,int *eof,void *data);
static int read_proc2(char *page,char **start,off_t offset,
					  int count,int *eof,void *data);
static void free_all(void);

static irqreturn_t  fpga_interrupt(int irq,void *stuff,
								   struct pt_regs *reg);
module_init(i2ctest_init);
module_exit(i2ctest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("I2C tester");


