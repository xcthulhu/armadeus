/* 
 ***********************************************************************
 * fpgaaccess.h
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 *
 * A simple driver for reading and writing on
 * fpga throught a character file /dev/fpgaaccess
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

#ifndef __FPGAACCESS_H_
#define __FPGAACCESS_H_

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#include <linux/config.h>
#endif

/* form module/drivers */
#include <linux/init.h>
#include <linux/module.h>

/* for file  operations */
#include <linux/fs.h>
#include <linux/cdev.h>

/* copy_to_user function */
#include <asm/uaccess.h>

/* readw() writew() */
#include <asm/io.h>

/* hardware addresses */
#include <asm/hardware.h>

#endif

/* for debugging messages*/
#define FPGAACCESS_DEBUG

#undef PDEBUG
#ifdef FPGAACCESS
# ifdef __KERNEL__
    /* for kernel spage */
#   define PDEBUG(fmt,args...) printk(KERN_DEBUG "fpgaaccess : " fmt, ##args)
# else
    /* for user space */
#   define PDEBUG(fmt,args...) printk(stderr, fmt, ##args)
# endif
#else
# define PDEBUG(fmt,args...) /* no debbuging message */
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("FPGA access device driver");

/* some defines */

#define FPGAACCESS_NAME "fpgaaccess"
#define FPGA_BASE_ADDR IMX_CS1_VIRT 
#define FPGA_SIZE      IMX_CS1_SIZE
#define N_DEV 1


/*************************/
/* main device structures */
/*************************/
struct fpgaaccess_dev{
  struct cdev cdev;     /* Char device structure */
  void * fpga_virtual_base_address;  /* fpga register base address*/
};

static dev_t devno;
struct fpgaaccess_dev *fpgaaccessdev;

/*************************************/
/* to read write fpgaaccess register */
/*************************************/
ssize_t fpgaaccess_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp);

ssize_t fpgaaccess_write(struct file *fildes, const char __user *
                  buff,size_t count, loff_t *offp);
int fpgaaccess_open(struct inode *inode, struct file *filp);

int fpgaaccess_release(struct inode *, struct file *filp);

struct file_operations fpgaaccess_fops = {
  .owner = THIS_MODULE,
  .read  = fpgaaccess_read,
  .write = fpgaaccess_write,
  .open  = fpgaaccess_open,
  .release = fpgaaccess_release,
};

/********************************/
/* init and release module      */
/********************************/

static int __init fpgaaccess_init(void);
static void __exit fpgaaccess_exit(void);

module_init(fpgaaccess_init);
module_exit(fpgaaccess_exit);
