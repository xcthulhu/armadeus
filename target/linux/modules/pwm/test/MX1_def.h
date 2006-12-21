/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2002 Motorola Semiconductors HK Ltd
 *
 */

/******************************************************************************

 C   H E A D E R   F I L E

 (c) Copyright Motorola Semiconductors Hong Kong Limited 2001-2002
 ALL RIGHTS RESERVED

*******************************************************************************

 Project Name : DBMX1 System Test Program
 Project No.  :
 Title        :
 Template Ver : 0.3
 File Name    : MX1_def.h
 Last Modified: Nov 14, 2001

 Description  : Definition header for DBMX1 silicon.

 Assumptions  : NA

 Dependency Comments : NA

 Project Specific Data : NA

******************************************************************************/
#ifndef MX1_DEF_INC
#define MX1_DEF_INC

/*************************** Header File Includes ****************************/
/********************************* Constants *********************************/
/******************************** Enumeration ********************************/
/****************************** Basic Data types *****************************/
/************************* Structure/Union Data types ************************/
/********************************** Macros ***********************************/
#define PLL_BASE				0xf021B000
#define PLL_CSCR				(PLL_BASE)
#define PLL_PCDR				(PLL_BASE+0x20)

//
// ;---------------------------------------;
// ; GPIO - PTA                            ;
// ; $0021_C000 to $0021_C0FF              ;
// ;---------------------------------------;
#define PTA_BASE_ADDR           0xf021C000
#define PTA_DDIR                PTA_BASE_ADDR
#define PTA_OCR1                (PTA_BASE_ADDR+0x04)
#define PTA_OCR2                (PTA_BASE_ADDR+0x08)
#define PTA_ICONFA1             (PTA_BASE_ADDR+0x0C)
#define PTA_ICONFA2             (PTA_BASE_ADDR+0x10)
#define PTA_ICONFB1             (PTA_BASE_ADDR+0x14)
#define PTA_ICONFB2             (PTA_BASE_ADDR+0x18)
#define PTA_DR                  (PTA_BASE_ADDR+0x1C)
#define PTA_GIUS                (PTA_BASE_ADDR+0x20)
#define PTA_SSR                 (PTA_BASE_ADDR+0x24)
#define PTA_ICR1                (PTA_BASE_ADDR+0x28)
#define PTA_ICR2                (PTA_BASE_ADDR+0x2C)
#define PTA_IMR                 (PTA_BASE_ADDR+0x30)
#define PTA_ISR                 (PTA_BASE_ADDR+0x34)
#define PTA_GPR                 (PTA_BASE_ADDR+0x38)
#define PTA_SWR                 (PTA_BASE_ADDR+0x3C)
#define PTA_PUEN                (PTA_BASE_ADDR+0x40)

//
// ;---------------------------------------;
// ; PWM                                   ;
// ; $0020_8000 to $0020_8FFF              ;
// ;---------------------------------------;
#define PWM1_BASE_ADDR          0xf0208000
#define PWMC1                   PWM1_BASE_ADDR
#define PWMS1                   (PWM1_BASE_ADDR+0x04)
#define PWMP1                   (PWM1_BASE_ADDR+0x08)
#define PWMCNT1                 (PWM1_BASE_ADDR+0x0C)
#define PWMTST1                 (PWM1_BASE_ADDR+0x10)

#endif
