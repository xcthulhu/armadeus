/*
 * as1531.c
 *
 * Platform data for driver as1531.
 *
 * Copyright (c) 2010 Fabien Marteau <fabien.marteau@armadeus.com>
 * sponsored by ARMadeus Systems.
 *
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/mutex.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <mach/common.h>
#include <mach/hardware.h>
#include <mach/iomux-mx1-mx2.h>
#include <mach/gpio.h>
#include <../mach-imx/include/mach/spi_imx.h>

#define AS1531_CS (GPIO_PORTE | 21)
/* spi3*/
#define AS1531_SPI_BUS_NUM 2


struct spi_device *as1531_spi_dev;

static int as1531_pins[] = {
	(AS1531_CS | GPIO_OUT | GPIO_GPIO),
};

static int as1531_init_gpio(void)
{
	gpio_set_value(AS1531_CS, 1);
	return mxc_gpio_setup_multiple_pins(as1531_pins, ARRAY_SIZE(as1531_pins), "as1531");
}

static void as1531_exit_gpio(void)
{
	mxc_gpio_release_multiple_pins(as1531_pins, ARRAY_SIZE(as1531_pins));
}

/* Chip select command for as1531 */
static void as1531_cs(u32 command)
{
	if (command == SPI_CS_DEASSERT)
		gpio_set_value(AS1531_CS, 1);
	else
		gpio_set_value(AS1531_CS, 0);
}

static struct spi_imx_chip as1531_hw = {
	.cs_control     = as1531_cs,
};

static struct spi_board_info spi_as1531_board[] = {
	{
		.modalias		= "as1531",
		.controller_data	= &as1531_hw,
		.max_speed_hz		= 1000000, /* 1MHz */
		.bus_num		= AS1531_SPI_BUS_NUM, 
		.mode			= SPI_MODE_3,
		.chip_select		= 0,
		.platform_data		= NULL,
	}
};


static int __init as1531_plat_init(void)
{
	if (as1531_init_gpio() < 0) {
		printk(KERN_ERR "Can't initialize gpio CS for as1531 spi\n");
		return -1;
	}

	as1531_spi_dev = spi_new_device(
			spi_busnum_to_master(AS1531_SPI_BUS_NUM),
			spi_as1531_board);
	if (as1531_spi_dev == NULL) {
		printk(KERN_ERR "Can't register as1531_spi_dev\n");
		goto register_error;
	}

	return 0;
register_error:
	return -1;
}

static void __exit as1531_plat_exit(void)
{
	spi_unregister_device(as1531_spi_dev);
	as1531_exit_gpio();
}

module_init(as1531_plat_init);
module_exit(as1531_plat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com>");
MODULE_DESCRIPTION("Platform data for AS1531 ADC");

