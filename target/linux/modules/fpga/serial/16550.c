/*
 * Initialisation Driver for OpenCore 16550 serial IP
 *   loaded in FPGA of the Armadeus boards.
 *
 * (C) Copyright 2008 Armadeus Systems
 * Author: Julien Boibessot <julien.boibessot@armadeus.com>
 *
 * Inspired from Au1x00 Init from Pantelis Antoniou
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/types.h>

#include <asm/io.h> // readb()


#define PORT(_base, _phys, _clock, _irq)   \
	{	                                   \
		.membase  = (void __iomem *)_base, \
		.mapbase  = _phys,                 \
		.irq      = _irq,                  \
		.uartclk  = _clock,                \
		.regshift = 1,                     \
		.iotype   = UPIO_MEM,              \
		.flags    = UPF_BOOT_AUTOCONF      \
	}


/* POD specific part */
#define UART_INPUT_CLOCK 96000000
#define UART1_BASE       0x10
#define UART2_BASE       0x20
#define UART1_IRQ        APF9328_FPGA_IRQ
#define UART2_IRQ        APF9328_FPGA_IRQ
/* End of POD specific part */


static struct plat_serial8250_port ocore_16550_data[] = {
	PORT( APF9328_FPGA_VIRT+UART1_BASE, APF9328_FPGA_PHYS+UART1_BASE, UART_INPUT_CLOCK, UART1_IRQ ),
	PORT( APF9328_FPGA_VIRT+UART2_BASE, APF9328_FPGA_PHYS+UART2_BASE, UART_INPUT_CLOCK, UART2_IRQ ),
	{ },
};

static struct platform_device ocore_16550_device = {
	.name = "serial8250", /* we use generic 8250/16550 Linux layer */
	.id   = PLAT8250_DEV_PLATFORM1,
	.dev  = {
		.platform_data = ocore_16550_data,
	},
};

static int __init ocore_16550_init(void)
{
	return platform_device_register( &ocore_16550_device );
}

static void __exit ocore_16550_exit(void)
{
	platform_device_unregister( &ocore_16550_device );
}

module_init(ocore_16550_init);
module_exit(ocore_16550_exit);

MODULE_AUTHOR("Julien Boibessot, <julien.boibessot@armadeus.com>");
MODULE_DESCRIPTION("8250 Linux layer registration module for 16550 OpenCore IP in Armadeus FPGA");
MODULE_LICENSE("GPL");
