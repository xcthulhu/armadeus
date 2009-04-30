/*
 * APF9328 Xilinx FPGA download support
 * Copyright (C) 2006-2009 Nicolas Colombain <nicolas.colombain@armadeus.com>
 * Copyright (C) 2006-2009 Eric Jarrige <eric.jarrige@armadeus.com>
 *                         Armadeus Project / Armadeus systems
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

#include <asm/io.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <mach/gpio.h>

#include "xilinx-fpga-loader.h"

#define FPGA_INIT	(GPIO_PORTB | 15)	/* FPGA init pin (SSI input)  */
#define FPGA_DONE	(GPIO_PORTB | 16)	/* FPGA done pin (SSI input)  */
#define FPGA_DIN	(GPIO_PORTB | 17)	/* FPGA data pin (SSI output) */
#define FPGA_PROGRAM	(GPIO_PORTB | 18)	/* FPGA prog pin (SSI output) */
#define FPGA_CLOCK	(GPIO_PORTB | 19)	/* FPGA clk pin  (SSI output) */


/*
 * Set the FPGA's active-low program line to the specified level
 */
static int apf9328_fpga_pgm( int assert )
{
	gpio_set_value(FPGA_PROGRAM, !assert);
	return assert;
}

/*
 * Set the FPGA's active-high clock line to the specified level
 */
static int apf9328_fpga_clk( int assert_clk )
{
	gpio_set_value(FPGA_CLOCK, assert_clk);
	return assert_clk;
}

/*
 * Test the state of the active-low FPGA INIT line.  Return 1 on INIT
 * asserted (low).
 */
static int apf9328_fpga_init( void )
{
	return(!gpio_get_value(FPGA_INIT));
}

/*
 * Test the state of the active-high FPGA DONE pin
 */
static int apf9328_fpga_done( void )
{
	return(gpio_get_value(FPGA_DONE));
}

/*
 * Set the FPGA's data line to the specified level
 */
static int apf9328_fpga_wr( int assert_write )
{
	gpio_set_value(FPGA_DIN, assert_write);
	return assert_write;
}

static int apf9328_fpga_pre( void )
{
	/* Initialize GPIO pins */
	imx_gpio_mode(FPGA_INIT | GPIO_GIUS | GPIO_DR | GPIO_IN);
	imx_gpio_mode(FPGA_DONE | GPIO_GIUS | GPIO_DR | GPIO_IN);
	imx_gpio_mode(FPGA_DIN  | GPIO_GIUS | GPIO_DR | GPIO_OUT);
	imx_gpio_mode(FPGA_PROGRAM | GPIO_GIUS | GPIO_DR | GPIO_OUT);
	imx_gpio_mode(FPGA_CLOCK | GPIO_GIUS | GPIO_DR | GPIO_OUT);
	return 1;
}


Xilinx_Spartan_Slave_Serial_fns fpga_fns = {
	.pre = apf9328_fpga_pre,
	.pgm = apf9328_fpga_pgm,
	.clk = apf9328_fpga_clk,
	.init = apf9328_fpga_init,
	.done = apf9328_fpga_done,
	.wr = apf9328_fpga_wr,
};

struct fpga_desc apf9328_fpga_desc = {
	.family = Xilinx_Spartan,
	.iface = slave_serial,
	.iface_fns = (void *) &fpga_fns
};

static struct platform_device fpga_device = {
	.name		= "fpgaloader",
	.id		= 0,
	.dev = {
		.platform_data	= &apf9328_fpga_desc,
	},
};

static struct platform_device *devices[] __initdata = {
	&fpga_device,
};

static int __init apf9328_fpga_initialize(void)
{
	platform_add_devices(devices, ARRAY_SIZE(devices));
	return 0;
}

device_initcall(apf9328_fpga_initialize);
