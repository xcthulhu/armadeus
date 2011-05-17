/*
 * Platform data for OpenCore 16750 2x serial IP
 * loaded in FPGA of the Armadeus boards.
 *
 * (C) Copyright 2008-2011 ARMadeus Systems
 * Author: Julien Boibessot <julien.boibessot@armadeus.com>
 *
 * Inspired from Au1x00 Init from Pantelis Antoniou
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/serial_8250.h>

#include <asm/io.h>
#ifndef CONFIG_MACH_APF9328 /* To remove when MX1 platform is merged */
#include <mach/fpga.h>
#endif

#define GSM_UART_INPUT_CLOCK   96000000
#define GSM_UART_BASE  0x20
#define GSM_UART_IRQ   IRQ_FPGA(0)

#define GPS_UART_INPUT_CLOCK   96000000
#define GPS_UART_BASE  0x40
#define GPS_UART_IRQ   IRQ_FPGA(1)


void plat_uart_release(struct device *dev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30) 
	pr_debug("device %s released\n", dev->bus_id);
#else
	pr_debug("device %s released\n", dev->init_name);
#endif
}

static struct plat_serial8250_port ocore_16750_uart0_data[] = {
	{
		.mapbase  = ARMADEUS_FPGA_BASE_ADDR + GSM_UART_BASE,
		.irq      = IRQ_FPGA(0),
		.uartclk  = 96000000,
		.regshift = 1,
		.iotype   = UPIO_MEM,
		.flags    = UPF_BOOT_AUTOCONF
	}
};

static struct plat_serial8250_port ocore_16750_uart1_data[] = {
	{
		.mapbase  = ARMADEUS_FPGA_BASE_ADDR + GPS_UART_BASE,
		.irq      = IRQ_FPGA(1),
		.uartclk  = 96000000,
		.regshift = 1,
		.iotype   = UPIO_MEM,
		.flags    = UPF_BOOT_AUTOCONF
	}
};

static struct platform_device ocore_16750_uart0_device = {
	.name	= "serial8250",
	.id	= 0,
	.dev	= {
		.release	= plat_uart_release,
		.platform_data	= ocore_16750_uart0_data,
	},
};

static struct platform_device ocore_16750_uart1_device = {
	.name	= "serial8250",
	.id	= 1,
	.dev	= {
		.release	= plat_uart_release,
		.platform_data	= ocore_16750_uart1_data,
	},
};

static int __init ocore_16750_init(void)
{
	int ret = -ENODEV;
	u16 id;

	ocore_16750_uart0_data[0].membase = ioremap(ocore_16750_uart0_data[0].mapbase, 0x20);
	if (!ocore_16750_uart0_data[0].membase) {
		printk(KERN_ERR "%s: ioremap failed\n", __func__);
		return -ENOMEM;
	}

	/* check if ID is correct for GSM_UART */
	id = readw(ocore_16750_uart0_data[0].membase + 0x08*2);
	if (id != 2) {
		printk(KERN_WARNING "For GSM_UART id:2 doesn't match with "
			   "id read %d,\n is device present ?\n", id);
		return -ENODEV;
	}

	ocore_16750_uart1_data[0].membase = ioremap(ocore_16750_uart1_data[0].mapbase, 0x20);
	if (!ocore_16750_uart1_data[0].membase) {
		printk(KERN_ERR "%s: ioremap failed\n", __func__);
		return -ENOMEM;
	}

	/* check if ID is correct for GPS_UART */
	id = readw(ocore_16750_uart1_data[0].membase + 0x08*2);
	if (id != 3) {
		printk(KERN_WARNING "For GPS_UART id:3 doesn't match with "
			   "id read %d,\n is device present ?\n", id);
		return -ENODEV;
	}

	ret = platform_device_register(&ocore_16750_uart0_device);
	if (ret < 0)
		return ret;

	ret = platform_device_register(&ocore_16750_uart1_device);
	if (ret < 0)
		return ret;

	return ret;
}

static void __exit ocore_16750_exit(void)
{
	platform_device_unregister(&ocore_16750_uart0_device);
	platform_device_unregister(&ocore_16750_uart1_device);
}

module_init(ocore_16750_init);
module_exit(ocore_16750_exit);

MODULE_AUTHOR("Julien Boibessot, <julien.boibessot@armadeus.com>");
MODULE_DESCRIPTION("Platform data for 16750 OpenCore IP in Armadeus FPGA");
MODULE_LICENSE("GPL");
