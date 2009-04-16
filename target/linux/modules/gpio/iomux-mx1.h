/*
 * Copyright (C) 2006-2008 Julien Boibessot <julien.boibessot@armadeus.com>
 *                         Armadeus Project / Armadeus Systems
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

#ifndef __IOMUX_MX1_H__
#define __IOMUX_MX1_H__

#define VA_GPIO_BASE   IO_ADDRESS(IMX_GPIO_BASE)
#define MXC_DDIR(x)    (0x00 + ((x) << 8))
#define MXC_OCR1(x)    (0x04 + ((x) << 8))
#define MXC_OCR2(x)    (0x08 + ((x) << 8))
#define MXC_ICONFA1(x) (0x0c + ((x) << 8))
#define MXC_ICONFA2(x) (0x10 + ((x) << 8))
#define MXC_ICONFB1(x) (0x14 + ((x) << 8))
#define MXC_ICONFB2(x) (0x18 + ((x) << 8))
#define MXC_DR(x)      (0x1c + ((x) << 8))
#define MXC_GIUS(x)    (0x20 + ((x) << 8))
#define MXC_SSR(x)     (0x24 + ((x) << 8))
#define MXC_ICR1(x)    (0x28 + ((x) << 8))
#define MXC_ICR2(x)    (0x2c + ((x) << 8))
#define MXC_IMR(x)     (0x30 + ((x) << 8))
#define MXC_ISR(x)     (0x34 + ((x) << 8))
#define MXC_GPR(x)     (0x38 + ((x) << 8))
#define MXC_SWR(x)     (0x3c + ((x) << 8))
#define MXC_PUEN(x)    (0x40 + ((x) << 8))

#endif /* __IOMUX_MX1_H__ */
