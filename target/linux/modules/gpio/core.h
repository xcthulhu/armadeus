/*
 * Armadeus i.MXL/i.MX27 GPIO management driver
 *
 * Copyright (C) 2006-2010 Julien Boibessot <julien.boibessot@armadeus.com>
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
/* direction: 1 output, 0 input */
#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)

#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int)

/* mode: 1 GPIO, 0 device function */
#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int)

#define GPIORDPULLUP	_IOR(PP_IOCTL, 0xF6, int)
#define GPIOWRPULLUP	_IOW(PP_IOCTL, 0xF7, int)

/* IRQ type: 0 none, 1 rising, 2 falling, 3 both */
#define GPIORDIRQMODE	_IOR(PP_IOCTL, 0xF8, int)
#define GPIOWRIRQMODE	_IOW(PP_IOCTL, 0xF9, int)

/* Pretend we're PPDEV for IOCTL */
#include <linux/ppdev.h>

#endif				/* __CORE_GPIO_H__ */
