/*
 * APF27 Xilinx FPGA download support
 * Copyright (C) 2009 Nicolas Colombain <nicolas.colombain@armadeus.com>
 *                    Armadeus Project / Armadeus systems
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

#include <linux/version.h>
#include <mach/iomux-mx1-mx2.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <mach/hardware.h>

#include "xilinx-fpga-loader.h"

#define FPGA_BASE_ADDR CS5_BASE_ADDR
#define FPGA_VIRT_ADDR  (IO_ADDRESS(FPGA_BASE_ADDR) + 0x0)

#define CFG_FPGA_PWR		(GPIO_PORTF | 19)	/* FPGA prog pin  */
#define CFG_FPGA_PRG		(GPIO_PORTF | 11)	/* FPGA prog pin  */
#define CFG_FPGA_CLK		(GPIO_PORTF | 15)	/* FPGA clk pin   */
#define CFG_FPGA_RDATA		0xD6000000		/* FPGA data addr  */
#define CFG_FPGA_WDATA		0xD6000000		/* FPGA data addr  */
#define CFG_FPGA_INIT		(GPIO_PORTF | 12)	/* FPGA init pin  */
#define CFG_FPGA_DONE		(GPIO_PORTF | 9)	/* FPGA done pin  */
#define CFG_FPGA_RW		(GPIO_PORTF | 21)	/* FPGA done pin  */
#define CFG_FPGA_CS		(GPIO_PORTF | 22)	/* FPGA done pin  */
#define CFG_FPGA_SUSPEND	(GPIO_PORTF | 10)	/* FPGA done pin  */
#define CFG_FPGA_RESET		(GPIO_PORTF | 7)	/* FPGA done pin  */

#define NB_TARGET_DESC 2


/* Initialize GPIO port before download */
int fpga_pre_fn (void)
{
	gpio_set_value(CFG_FPGA_PWR, 1);
	gpio_set_value(CFG_FPGA_PRG, 1);
	gpio_set_value(CFG_FPGA_CLK, 1);
	gpio_set_value(CFG_FPGA_RW, 1);
	gpio_set_value(CFG_FPGA_CS, 1);
	gpio_set_value(CFG_FPGA_SUSPEND, 0);
	gpio_set_value(CFG_FPGA_RESET, 0);

	/* Initialize GPIO pins */
	mxc_gpio_mode(CFG_FPGA_PWR | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_INIT | GPIO_IN | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_DONE | GPIO_IN | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_PRG | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_CLK | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_RW | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_CS | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_SUSPEND | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);
	mxc_gpio_mode(CFG_FPGA_RESET | GPIO_OUT | GPIO_PUEN | GPIO_GPIO);

	gpio_set_value(CFG_FPGA_PWR, 0);
	return 1;
}

/*
 * Set the FPGA's active-low program line to the specified level
 */
int fpga_pgm_fn (int assert)
{
	gpio_set_value( CFG_FPGA_PRG, !assert);
	return assert;
}

/*
 * Set the FPGA's active-high clock line to the specified level
 */
int fpga_clk_fn (int assert_clk)
{
	gpio_set_value( CFG_FPGA_CLK, !assert_clk);
	return assert_clk;
}

/*
 * Test the state of the active-low FPGA INIT line.  Return 1 on INIT
 * asserted (low).
 */
int fpga_init_fn (void)
{
	int value;
	value = gpio_get_value(CFG_FPGA_INIT);
	return !value;
}

/*
 * Test the state of the active-high FPGA DONE pin
 */
int fpga_done_fn (void)
{
	return(gpio_get_value(CFG_FPGA_DONE));
}

/*
 * Set the FPGA's wr line to the specified level
 */
int fpga_wr_fn (int assert_write)
{
	gpio_set_value( CFG_FPGA_RW, !assert_write);
	return assert_write;
}

int fpga_cs_fn (int assert_cs)
{
	gpio_set_value( CFG_FPGA_CS, !assert_cs);
	return assert_cs;
}

int fpga_wdata_fn ( unsigned char data )
{
	__raw_writew(data, FPGA_VIRT_ADDR);
	return data;
}

int fpga_busy_fn (void)
{
	return 0;
}
 
int fpga_post_fn (void)
{
	mxc_gpio_mode (CFG_FPGA_RW | GPIO_PF | GPIO_PUEN);
	mxc_gpio_mode (CFG_FPGA_CS | GPIO_PF | GPIO_PUEN);
	mxc_gpio_mode (CFG_FPGA_CLK | GPIO_PF | GPIO_PUEN);
	gpio_set_value(CFG_FPGA_PRG, 1);
	return 1;
}

int fpga_abort_fn (void)
{
	fpga_post_fn();
	gpio_set_value(CFG_FPGA_PWR, 1);
	return 1;
}

/* Note that these are pointers to code that is in Flash.  They will be
 * relocated at runtime.
 * Spartan2 code is used to download our Spartan 3 :) code is compatible.
 * Just take care about the file size
 */
Xilinx_Spartan3_Slave_Parallel_fns fpga_fns = {
	fpga_pre_fn,
	fpga_pgm_fn,
	fpga_clk_fn,
	fpga_init_fn,
	fpga_done_fn,
	fpga_wr_fn,
	fpga_cs_fn,
	fpga_wdata_fn,
	fpga_busy_fn,
	fpga_abort_fn,
	fpga_post_fn,
};

Xilinx_desc target_fpga_desc[NB_TARGET_DESC] = {
	{
		Xilinx_Spartan,
		slave_parallel,
		1196128l/8,
		(void *) &fpga_fns 
	},
	{
		Xilinx_Spartan,
		slave_parallel,
		1887216l/8,
		(void *) &fpga_fns 
	},
};
