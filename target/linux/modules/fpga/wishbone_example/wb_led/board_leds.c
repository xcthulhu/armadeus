/*
 * Platform data for generic LED IP driver
 *
 * (c) Copyright 2008-2011 The Armadeus Project - ARMadeus Systems
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

#ifndef CONFIG_MACH_APF9328 /* to be removed when MX1 platform is merged */
#include <mach/fpga.h>
#endif

#include "led.h"

static struct resource led0_resources[] = {
	[0] = {
		.start	= ARMADEUS_FPGA_BASE_ADDR + 0x8,
		.end	= ARMADEUS_FPGA_BASE_ADDR + 0x8 + 0x4,
		.flags	= IORESOURCE_MEM,
	},
};

static struct plat_led_port plat_led0_data = {
	.name		= "LED0",
	.num		= 0,
	.idnum		= 2,
	.idoffset	= 0x01 * (16 / 8),
};

void plat_led_release(struct device *dev)
{
	dev_dbg(dev, "released\n");
}

static struct platform_device plat_led0_device = {
	.name	= "led",
	.id	= 0,
	.dev	= {
		.release	= plat_led_release,
		.platform_data	= &plat_led0_data
	},
	.num_resources	= ARRAY_SIZE(led0_resources),
	.resource	= led0_resources,
};

static int __init sled_init(void)
{
	return platform_device_register(&plat_led0_device);
}

static void __exit sled_exit(void)
{
	platform_device_unregister(&plat_led0_device);
}

module_init(sled_init);
module_exit(sled_exit);

MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com>");
MODULE_DESCRIPTION("Driver to blink some LEDs on FPGA");
MODULE_LICENSE("GPL");
