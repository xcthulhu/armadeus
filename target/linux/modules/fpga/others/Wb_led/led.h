/*
 ***********************************************************************
 *
 * (c) Copyright 2007    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Driver for Wb_led IP
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


#ifndef __LED_H__
#define __LED_H__

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#include <linux/config.h>
#endif

/* form module/drivers */
#include <linux/init.h>
#include <linux/module.h>

/* linux device model 
*#include <linux/kobject.h>
*#include <linux/sysfs.h>
*#include <linux/sysdev.h>
*#include <linux/device.h>
*/

/* for file  operations */
#include <linux/fs.h>
#include <linux/cdev.h>

/* copy_to_user function */
#include <asm/uaccess.h>

/* request_mem_region */
#include <linux/ioport.h>

/* readw() writew() */
#include <asm/io.h>

/* hardware addresses */
#include <asm/hardware.h>

/* for platform device */
#include <linux/platform_device.h>

#endif

/* for debugging messages*/
#define LED_DEBUG

#undef PDEBUG
#ifdef LED_DEBUG
# ifdef __KERNEL__
    /* for kernel spage */
#   define PDEBUG(fmt,args...) printk(KERN_DEBUG "LED : " fmt, ##args)
# else
    /* for user space */
#   define PDEBUG(fmt,args...) printk(stderr, fmt, ##args)
# endif
#else
# define PDEBUG(fmt,args...) /* no debbuging message */
#endif


#define FPGA_BASE_ADDR IMX_CS1_PHYS
#define FPGA_MEM_SIZE  IMX_CS1_SIZE

#define LED_NUMBER 2

/* platform device */
struct plat_led_port{
    const char   *name;    /* instance name  */
    int          num;      /* instance number */
    unsigned int membase;  /* ioremap base address */
};

static struct plat_led_port plat_led_data[] = {
    {
        .name    = "led0",
        .num     = 0,
        .membase = 0x04
    },
    {
        .name    = "led1",
        .num     = 1,
        .membase = 0x06
    },
    {
        .name    = NULL
    },
};

static struct platform_device plat_led_device = {
    .name = "led",
    .id   = 0,
    .dev  = {
        .platform_data = plat_led_data
    },
};


static int led_probe(struct platform_device *plat_led_device);
static int led_remove(struct platform_device *plat_led_device);

static struct platform_driver plat_led_driver = {
    .probe      = led_probe,
    .remove     = led_remove,
    .driver     = {
        .name    = "led",
        .owner   = THIS_MODULE,
    },
};

/************************************
 * module init and exit
 ***********************************/

static int __init led_init(void){
    int ret;
    PDEBUG("init Led platform driver\n");
    PDEBUG("Platform driver name %s\n",plat_led_driver.driver.name);
    ret = platform_driver_register(&plat_led_driver);
    if(ret){
        printk(KERN_ERR "Platform driver register error\n");
        return ret;
    }
    PDEBUG("Driver registered\n");
    ret = platform_device_add(&plat_led_device);
    if(ret){
        printk(KERN_ERR "Platform device adding problem\n");
        return ret;
    }
    return 0;
}

static void __exit led_exit(void){
    PDEBUG("Exit function \n");
    platform_driver_unregister(&plat_led_driver);
    PDEBUG("driver unregistered\n");
    //platform_device_unregister(&plat_led_device);
    //PDEBUG("device unregistered\n");

}

/*************************/
/* main device structure */
/*************************/
struct led_dev{
    char *name;            /* the name of the instance */
    struct cdev cdev;     /* Char device structure */
    void * fpga_virtual_base_address;  /* fpga register base address*/
    unsigned int membase;  /* base address for instance relative to
                              fpga_virtual_base_address */
    dev_t devno;            /* to store Major and minor numbers */
};

static struct led_dev led_device[LED_NUMBER];

/******************************/
/* to read write led register */
/******************************/
ssize_t led_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp);

ssize_t led_write(struct file *fildes, const char __user *
                  buff,size_t count, loff_t *offp);
int led_open(struct inode *inode, struct file *filp);

int led_release(struct inode *, struct file *filp);

static struct file_operations led_fops = {
  .owner = THIS_MODULE,
  .read  = led_read,
  .write = led_write,
  .open  = led_open,
  .release = led_release,
};

/****************************
 * personnal functions
 ***************************/

static int free_all(struct plat_led_port *);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("Led device driver");


