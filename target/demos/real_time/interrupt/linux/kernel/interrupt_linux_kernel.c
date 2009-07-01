/*
* Small Linux driver for handling interrupt from a GPIO and toggling
* an other one at each occurancy.
*
* Copyright (C) 2009 Armadeus Systems / Armadeus Project
* Author: Gwenhael GOAVEC-MEROU <gwenhael.goavec-merou@armadeus.com>
*
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

#include <linux/module.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/gpio.h>

#include "../../../common_kernel.h"


MODULE_AUTHOR("Gwenhael GOAVEC MEROU");
MODULE_DESCRIPTION("irq kernel test");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");

static int iomask = 0x00;

/* irq handler */
static irqreturn_t test_interrupt_handler(int irqn, void *dev)
{
	gpio_set_value(INTERRUPT_OUTPUT_GPIO, iomask);
	iomask^=1;

	return IRQ_HANDLED;
}

static int __init test_irq_init(void)
{
	int err;

	err = request_irq(INTERRUPT_INPUT_NB, test_interrupt_handler, 0, "test irq", NULL);
	if (err) {
		printk(KERN_INFO "Error while requesting IRQ %d\n", INTERRUPT_INPUT_NB);
		return err;
	}
	set_irq_type(INTERRUPT_INPUT_NB, IRQF_TRIGGER_FALLING); 

	return 0;
}

static void __exit test_irq_exit(void)
{
	free_irq(INTERRUPT_INPUT_NB, NULL);
}

module_init(test_irq_init);
module_exit(test_irq_exit);

