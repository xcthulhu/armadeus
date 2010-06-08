/*
 * Armadeus i.MXL GPIO management driver
 *
 * Copyright (C) 2006-2008 Julien Boibessot <julien.boibessot@armadeus.com>
 *                         Nicolas Colombain <nicolas.colombain@armadeus.com>
 *                         Armadeus Project / Armadeus Systems
 *
 * Inspired by a lot of other GPIO management systems...
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
 *
 */

#ifndef __CORE_GPIO_H__
#define __CORE_GPIO_H__

/* IOCTL */
#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)/* Read/write bitmask that determine*/
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)/*in/out pins (1 output, 0 input) */

#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int)

#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int)

#define GPIORDPULLUP	_IOR(PP_IOCTL, 0xF6, int)
#define GPIOWRPULLUP	_IOW(PP_IOCTL, 0xF7, int)

#define GPIORDIRQMODE_H	_IOR(PP_IOCTL, 0xF8, int) /* only for /dev/gpio/gpioX 	*/
#define GPIORDIRQMODE_L	_IOR(PP_IOCTL, 0xF9, int) /*							*/
#define GPIOWRIRQMODE_H	_IOW(PP_IOCTL, 0xFA, int) /*							*/
#define GPIOWRIRQMODE_L	_IOW(PP_IOCTL, 0xFB, int) /*							*/

#define GPIORDISR 	_IOR(PP_IOCTL, 0xFC, int)
#define GPIOWRISR	_IOW(PP_IOCTL, 0xFD, int)

#define GPIORDIRQMODE	_IOR(PP_IOCTL, 0xFE, int) /* only for /dev/gpio/PXx */
#define GPIOWRIRQMODE	_IOW(PP_IOCTL, 0xFF, int) /*						*/

/* Pretend we're PPDEV for IOCTL */
#include <linux/ppdev.h>

#endif /* __CORE_GPIO_H__ */
