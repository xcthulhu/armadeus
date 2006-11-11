/*
 **********************************************************************
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

#ifndef __CORE_GPIO_H__
#define __CORE_GPIO_H__

#include <linux/config.h>
#include <config/modversions.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/pci.h>

// Proc filenames:
#define GPIO_PROC_DIRNAME         "driver/generic"
//
#define GENERIC_PROC_FILENAME  GPIO_PROC_DIRNAME "/toto"
/*#define GPIO_PROC_PORTB_FILENAME  GPIO_PROC_DIRNAME "/portB"
#define GPIO_PROC_PORTC_FILENAME  GPIO_PROC_DIRNAME "/portC"
#define GPIO_PROC_PORTD_FILENAME  GPIO_PROC_DIRNAME "/portD"*/

// By default, we use dynamic allocation of major numbers -> MAJOR = 0
#define GPIO_MAJOR 0
#define GPIO_MAX_MAJOR 254
#define GPIO_MAX_MINOR 4 

// IOCTL 
#define GPIORDDIRECTION _IOR(PP_IOCTL, 0xF0, int)  //Read/write bitmask that determines input/output pins (1 means output, 0 input)
#define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA      _IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA      _IOW(PP_IOCTL, 0xF3, int)

#define MAX_NUMBER_OF_PINS 32
//#define OUR_NAME       "Armadeus_gpio"
#define DRIVER_NAME    "Armadeus GPIOs driver"
#define DRIVER_VERSION "v0.0.1"

// Pretend we're PPDEV for IOCTL
#include <linux/ppdev.h>


#endif // __CORE_GPIO_H__
