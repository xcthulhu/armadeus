/*
 * APF51 Xilinx FPGA download support
 * Copyright (C) 2011 Nicolas Colombain <nicolas.colombain@armadeus.com>
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
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <asm/io.h>
#include <mach/iomux-mx51.h>
#include <mach/fpga.h>
#include <mach/hardware.h>
#include "xilinx-fpga-loader.h"

#define MXC_CCM_CBCDR		0x14
#define MXC_CS1RCR1_ADDR	0x20
#define MXC_CS1WCR1_ADDR	0x28

#define	CONFIG_SYS_FPGA_PWR (2*32 + 3)  /*MX51_PAD_DI1_D0_CS__GPIO3_3*/
#define	CONFIG_SYS_FPGA_PRG (3*32 + 9)  /*MX51_PAD_CSI2_D12__GPIO4_9*/
#define	CONFIG_SYS_FPGA_INIT (3*32 + 11) /*MX51_PAD_CSI2_D18__GPIO4_11*/
#define	CONFIG_SYS_FPGA_DONE (3*32 + 10) /*MX51_PAD_CSI2_D13__GPIO4_10*/
#define	CONFIG_SYS_FPGA_SUSPEND (2*32 + 6) /*MX51_PAD_DISPB2_SER_DIO__GPIO3_6*/


u32 temp_clk; /* use to restore the emi_clk_sel after donwload */
u32 temp_rcr1;
u32 temp_wcr1;
void __iomem *cs1_base;


/* Initialize GPIO port before download */
int apf51_fpga_pre(void)
{
#define EMI_CLK_SEL		1<<26

	/* change emi_clk_sel to ensure blck smaller than 50MHz */
	temp_clk = __raw_readl( MX51_IO_ADDRESS(MX51_CCM_BASE_ADDR)+ MXC_CCM_CBCDR );
	__raw_writel( temp_clk | EMI_CLK_SEL, MX51_IO_ADDRESS(MX51_CCM_BASE_ADDR)+ MXC_CCM_CBCDR );

	temp_rcr1 =  __raw_readl( MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1RCR1_ADDR );
	__raw_writel( 0x01000010, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1RCR1_ADDR );

	temp_wcr1 = __raw_readl(MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1WCR1_ADDR);
	__raw_writel( 0x01000008, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1WCR1_ADDR );

	/* FPGA PROG */
	gpio_direction_output(CONFIG_SYS_FPGA_PRG, 1);

	/* FPGA PWR */
	gpio_direction_output(CONFIG_SYS_FPGA_PWR, 1);

	/* FPGA SUSPEND */
	gpio_direction_output(CONFIG_SYS_FPGA_SUSPEND, 1);

	/* FPGA DONE */
	gpio_direction_input(CONFIG_SYS_FPGA_DONE);

	/* FPGA INIT# */
	gpio_direction_input(CONFIG_SYS_FPGA_INIT);

	cs1_base = ioremap(MX51_CS1_BASE_ADDR, SZ_4K);

	return 0;
}

/*
 * Set the FPGA's active-low program line to the specified level
 */
int apf51_fpga_pgm(int assert)
{
	gpio_direction_output(CONFIG_SYS_FPGA_PRG, !assert);
	return assert;
}

/*
 * Set the FPGA's active-high clock line to the specified level
 */
int apf51_fpga_clk(int assert_clk)
{
	return assert_clk;
}

/*
 * Test the state of the active-low FPGA INIT line.  Return 1 on INIT
 * asserted (low).
 */
int apf51_fpga_init(void)
{
	if (gpio_get_value(CONFIG_SYS_FPGA_INIT))
		return 0;
	return 1;
}

/*
 * Test the state of the active-high FPGA DONE pin
 */
int apf51_fpga_done(void)
{
	return gpio_get_value(CONFIG_SYS_FPGA_DONE);
}

/*
 * Set the FPGA's wr line to the specified level
 */
int apf51_fpga_wr(int assert_write)
{
	return assert_write;
}

int apf51_fpga_cs(int assert_cs)
{
	return assert_cs;
}

int apf51_fpga_wdata( unsigned char data )
{
	static u32 temp;
	static u32 index = 0;

	switch( index++ ){
		case 0: temp = data << 8; break;
		case 1: temp = temp + data; break;
		case 2: temp = temp + (data << 24); break;
		default:
			__raw_writel(temp + (data << 16), cs1_base);
			index = 0;
			break;
	}

	return data;
}

int apf51_fpga_busy(void)
{
	return 0;
}

int apf51_fpga_post(void)
{
	/* restore emi_clk_sel */
	__raw_writel( temp_clk, MX51_IO_ADDRESS(MX51_CCM_BASE_ADDR)+ MXC_CCM_CBCDR );
	__raw_writel( temp_rcr1, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1RCR1_ADDR );
	__raw_writel( temp_wcr1, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1WCR1_ADDR );

	iounmap(cs1_base);

	return 0;
}

int apf51_fpga_abort(void)
{
	__raw_writel( temp_clk, MX51_IO_ADDRESS(MX51_CCM_BASE_ADDR)+ MXC_CCM_CBCDR );
	__raw_writel( temp_rcr1, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1RCR1_ADDR );
	__raw_writel( temp_wcr1, MX51_IO_ADDRESS(MX51_WEIM_BASE_ADDR) + MXC_CS1WCR1_ADDR );

	iounmap(cs1_base);

	return 1;
}

/* Spartan2 code is used to download our Spartan 3: code is compatible.
 * Just take care about the file size
 */
Xilinx_Spartan3_Slave_Parallel_fns fpga_fns = {
	.pre = apf51_fpga_pre,
	.pgm = apf51_fpga_pgm,
	.clk = apf51_fpga_clk,
	.init = apf51_fpga_init,
	.done = apf51_fpga_done,
	.wr = apf51_fpga_wr,
	.cs = apf51_fpga_cs,
	.wdata = apf51_fpga_wdata,
	.busy = apf51_fpga_busy,
	.abort = apf51_fpga_abort,
	.post = apf51_fpga_post,
};

struct fpga_desc apf51_fpga_desc = {
	.family = Xilinx_Spartan,
	.iface = slave_parallel,
	.iface_fns = (void *) &fpga_fns
};

static struct platform_device fpga_device = {
	.name		= "fpgaloader",
	.id		= 0,
	.dev = {
		.platform_data	= &apf51_fpga_desc,
	},
};

static struct platform_device *devices[] __initdata = {
	&fpga_device,
};

static int __init apf51_fpga_initialize(void)
{
	platform_add_devices(devices, ARRAY_SIZE(devices));

	return 0;
}

device_initcall(apf51_fpga_initialize);

