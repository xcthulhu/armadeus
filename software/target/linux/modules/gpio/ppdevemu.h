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

#ifndef __PPDEV_EMU_H__
#define __PPDEV_EMU_H__

#include <linux/config.h>
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

#define PPDEV_PROC_DIRNAME           "driver/ppdev"
#define PPDEV_PROC_FILENAME          PPDEV_PROC_DIRNAME "/infos"

/*#define LED_PROC_FILENAME         "driver/armadeus_error_led"
#define SETTINGS_PROC_FILENAME    "driver/portBdir"
#define TEMPERATURE_PROC_FILENAME "driver/soekris_temp"
#define VOLTAGE_PROC_FILENAME     "driver/soekris_voltage"*/
#define DRIVER_VERSION            "v0.0"
//#define VERSION                   "soekris"

/* by default, we use dynamic allocation of major numbers */
#define PPDEV_MAJOR 0
#define PPDEV_MAX_MAJOR 254

// /* minor numbers used*/
// #define MINOR_BYTE     0        /* access to GPIO0-GPIO7 */
// #define MINOR_FULL     1        /* access to GPIO0-GPIOxx (driver dependant) */
// #define MINOR_LED      254      /* access to the error LED */
// #define GPIO_MAX_MINOR 4 
#define PPDEV_MINOR 0
#define PPDEV_MAX_MINOR 4

// /* Read/write bitmask that determines input/output pins (1 means output, 0 input) */
// #define GPIORDDIRECTION _IOR(PP_IOCTL, 0xF0, int)
// #define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)
// 
// /* Read/write data */
// #define GPIORDDATA _IOR(PP_IOCTL, 0xF2, int)
// #define GPIOWRDATA _IOW(PP_IOCTL, 0xF3, int)

// #define MAX_NUMBER_OF_PINS 32
// #define TEMPERATURE_BUFFER_SIZE 256
// #define VOLTAGE_BUFFER_SIZE 512

#define PPDEV_DEVICE_NAME "Armadeus_ppdev"

// Pretend we're PPDEV for IOCTL
#include <linux/ppdev.h>


#endif // __PPDEV_EMU_H__
