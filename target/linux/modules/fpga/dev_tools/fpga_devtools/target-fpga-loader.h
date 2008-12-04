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

  (c) Copyright 2006    Armadeus project
  Eric Jarrige <eric.jarrige@armadeus.com>
  Nicolas Colombain <nicolas.colombain@armadeus.com>
  Target Xilinx FPGA support

*/
#ifndef __TARGET_FPGA_LOADER_H__
#define __TARGET_FPGA_LOADER_H__


#include "xilinx-fpga-loader.h"
#include <asm/arch/imx-regs.h>
#include <asm/io.h>

#define FPGA_INIT	    (GPIO_PORTB | 15)	/* FPGA init pin (SSI input)  */
#define FPGA_DONE	    (GPIO_PORTB | 16)	/* FPGA done pin (SSI input)  */
#define FPGA_DIN	    (GPIO_PORTB | 17)	/* FPGA data pin (SSI output) */
#define FPGA_PROGRAM	(GPIO_PORTB | 18)	/* FPGA prog pin (SSI output) */
#define FPGA_CLOCK	    (GPIO_PORTB | 19)	/* FPGA clk pin  (SSI output) */

#define NB_TARGET_DESC 2

#endif // __TARGET_FPGA_LOADER_H__
