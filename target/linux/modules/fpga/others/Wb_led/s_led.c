/*
 ***********************************************************************
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Specific led driver for generic led driver 
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

#ifndef __S_LED_H__
#define __S_LED_H__

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

#include"led.h"

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
    { },
};

static struct platform_device plat_led_device = {
    .name = "led",
    .id   = 0,
    .dev  = {
        .platform_data = plat_led_data
    },
};

static int __init sled_init(void){
    PDEBUG("Driver registered\n");
    return platform_device_register(&plat_led_device);
}

static void __exit sled_exit(void){
    platform_device_unregister(&plat_led_device);
}

module_init(sled_init);
module_exit(sled_exit);

MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com>");
MODULE_DESCRIPTION("Driver to blink blink some led");
MODULE_LICENSE("GPL");

