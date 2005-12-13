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

  (c) Copyright 2005    Armadeus project

*/

#ifndef __FPGA_GPIO_H__
#define __FPGA_GPIO_H__

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


#define FPGA_PROC_DIRNAME   "driver/fpga"
#define FPGA_PROC_FILENAME  FPGA_PROC_DIRNAME "spartan"

#define FPGA_DRIVER_VERSION "v0.0.1"
#define FPGA_DRIVER_NAME    "Armadeus fpga"

// By default, we use dynamic allocation of major numbers
#define FPGA_MAJOR 0
#define FPGA_MAX_MINOR 254

/* minor numbers used*/
#define GPIO_MAX_MINOR 4 

// Read/write bitmask that determines input/output pins (1 means output, 0 input)
#define GPIORDDIRECTION _IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)

// Read/write data
#define GPIORDDATA _IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA _IOW(PP_IOCTL, 0xF3, int)

#define MAX_NUMBER_OF_PINS 32
#define TEMPERATURE_BUFFER_SIZE 256
#define VOLTAGE_BUFFER_SIZE 512


#define FPGA_IOCTL 0x12345678   // !! TO BE BETTER DEFINED !!

#endif // __FPGA_GPIO_H__
