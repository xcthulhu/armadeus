/*
 * Platform data for generic button IP driver
 *
 * (c) Copyright 2008-2011    The Armadeus Project - ARMadeus Systems
 * Fabien Marteau <fabien.marteau@armadeus.com>
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
 */

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <mach/hardware.h>
#ifndef CONFIG_MACH_APF9328 /* To remove when MX1 platform is merged */
#include <mach/fpga.h>
#endif

#include "button.h"

#define BUTTON0_IRQ   IRQ_FPGA(0)

static struct resource button0_resources[] = {
	[0] = {
		.start	= ARMADEUS_FPGA_BASE_ADDR + 0xc,
		.end	= ARMADEUS_FPGA_BASE_ADDR + 0xc + 0x4,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= BUTTON0_IRQ,
		.end	= BUTTON0_IRQ,
		.flags	= IORESOURCE_IRQ,
	}
};

static struct plat_button_port plat_button0_data = {
	.name		= "BUTTON0",
	.num		= 0,
	.idnum		= 3,
	.idoffset	= 0x00 * (16 / 8)
};

void plat_button_release(struct device *dev)
{
	dev_dbg(dev, "released\n");
}

static struct platform_device plat_button0_device = {
	.name		= "button",
	.id		= 0,
	.dev		= {
		.release	= plat_button_release,
		.platform_data	= &plat_button0_data
	},
	.num_resources	= ARRAY_SIZE(button0_resources),
	.resource	= button0_resources,
};

static int __init board_button_init(void)
{
	return platform_device_register(&plat_button0_device);
}

static void __exit board_button_exit(void)
{
	platform_device_unregister(&plat_button0_device);
}

module_init(board_button_init);
module_exit(board_button_exit);

MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com>");
MODULE_DESCRIPTION("Board specific button driver");
MODULE_LICENSE("GPL");
