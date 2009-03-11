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
#define GPIORDDIRECTION _IOR(PP_IOCTL, 0xF0, int)  /* Read/write bitmask that determines input/output pins (1 means output, 0 input) */
#define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA      _IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA      _IOW(PP_IOCTL, 0xF3, int)

/* Pretend we're PPDEV for IOCTL */
#include <linux/ppdev.h>


#endif /* __CORE_GPIO_H__ */
