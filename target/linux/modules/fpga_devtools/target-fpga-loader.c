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

#include "target-fpga-loader.h"
#include "xilinx-fpga-loader.h"
#include <asm/arch/hardware.h>
#include <asm/io.h>

#define GPIO_GPIO GPIO_OCR_MASK // gpio mode

#define GPIO_PORT(x)  ((x >> 5) & 3)
#define GPIO_SET(x)   (DR(GPIO_PORT(x)) |= (1<<(x & GPIO_PIN_MASK)))
#define GPIO_CLEAR(x) (DR(GPIO_PORT(x)) &= ~(1<<(x & GPIO_PIN_MASK)))
#define GPIO_WRITE(x,y) ( y ? GPIO_SET(x) : GPIO_CLEAR(x) )
#define GPIO_READ(x)  ((SSR (GPIO_PORT(x)) & (1<<(x & GPIO_PIN_MASK))))

/*
 * Set the FPGA's active-low program line to the specified level
 */
int fpga_pgm_fn( int assert )
{
	GPIO_WRITE( FPGA_PROGRAM, !assert);
	return assert;
}

/*
 * Set the FPGA's active-high clock line to the specified level
 */
int fpga_clk_fn( int assert_clk )
{
	GPIO_WRITE( FPGA_CLOCK, assert_clk);
	return assert_clk;
}

/*
 * Test the state of the active-low FPGA INIT line.  Return 1 on INIT
 * asserted (low).
 */
int fpga_init_fn( void )
{
	return(!GPIO_READ(FPGA_INIT));
}

/*
 * Test the state of the active-high FPGA DONE pin
 */
int fpga_done_fn( void )
{
	return(GPIO_READ(FPGA_DONE));
}

/*
 * Set the FPGA's data line to the specified level
 */
int fpga_wr_fn( int assert_write )
{
	GPIO_WRITE( FPGA_DIN, assert_write);
	return assert_write;
}

int fpga_pre_fn( void )
{
	// Initialize GPIO pins
	imx_gpio_mode (FPGA_INIT | GPIO_GPIO | GPIO_IN );
	imx_gpio_mode (FPGA_DONE | GPIO_GPIO | GPIO_IN );
	imx_gpio_mode (FPGA_DIN  | GPIO_GPIO | GPIO_OUT );
	imx_gpio_mode (FPGA_PROGRAM | GPIO_GPIO | GPIO_OUT );
	imx_gpio_mode (FPGA_CLOCK | GPIO_GPIO | GPIO_OUT );
	return 1;
}


Xilinx_Spartan_Slave_Serial_fns fpga_fns = {
	fpga_pre_fn,
	fpga_pgm_fn,
	fpga_clk_fn,
	fpga_init_fn,
	fpga_done_fn,
	fpga_wr_fn,
};

Xilinx_desc target_fpga_desc[NB_TARGET_DESC] = {
	
    { // first supported configuration (default)
        Xilinx_Spartan,
	    slave_serial,
	    XILINX_XC3S200_SIZE,
	    (void *) &fpga_fns
    },

   	{ // second one
        Xilinx_Spartan,
	    slave_serial,
	    XILINX_XC3S400_SIZE,
	    (void *) &fpga_fns
     }
};

