/*
 * Driver for the IRQ manager loaded in FPGA of the Armadeus boards
 *
 * (C) Copyright 2008 Armadeus Systems
 * Author: Julien Boibessot <julien.boibessot@armadeus.com>
 *
 * Inspired of linux/arch/arm/mach-imx/irq.c
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/interrupt.h>

#include <asm/hardware.h>
#include <asm/arch/irqs.h>
#include <asm/irq.h>
#include <asm/io.h> // readb() & Co

#include <asm/mach/irq.h>

#if 0
#define DEBUG_FPGA_IRQ(fmt...)	printk(fmt)
#else
#define DEBUG_FPGA_IRQ(fmt...)	do { } while (0)
#endif


/* --- Things to be configured by POD --- */
#define FPGA_IMR (APF9328_FPGA_VIRT + 0x00) /* Interrupt Mask Register */
#define FPGA_ISR (APF9328_FPGA_VIRT + 0x02) /* Interrupt Status Register */
#define NB_IT    (16)
/* -------------------------------------- */


static int
imx_fpga_irq_type(unsigned int _irq, unsigned int type)
{
	return 0;
}

static void
imx_fpga_ack_irq(unsigned int irq)
{
	int shadow;

	shadow = 1 << ((irq - IRQ_FPGA_START) % NB_IT);
	DEBUG_FPGA_IRQ("%s: irq %d ack:0x%x\n", __FUNCTION__, irq, shadow);
	writew( shadow, FPGA_ISR );

	/* if last IT ack GPIO global IRQ */
	if( readw(FPGA_ISR) == 0 )
		ISR(IRQ_TO_REG(APF9328_FPGA_IRQ)) = 1 << (APF9328_FPGA_IRQ - IRQ_GPIOA(0));
}

static void
imx_fpga_mask_irq(unsigned int irq)
{
	int shadow;

	shadow = readw(FPGA_IMR);
	shadow &= ~( 1 << ((irq - IRQ_FPGA_START) % NB_IT));
	DEBUG_FPGA_IRQ("%s: irq %d mask:0x%x\n", __FUNCTION__, irq, shadow);
	writew( shadow, FPGA_IMR );
}

static void
imx_fpga_unmask_irq(unsigned int irq)
{
	int shadow;

	shadow = readw(FPGA_IMR);
	shadow |= 1 << ((irq - IRQ_FPGA_START) % NB_IT);
	DEBUG_FPGA_IRQ("%s: irq %d mask:0x%x\n", __FUNCTION__, irq, shadow);
	writew( shadow, FPGA_IMR );
}

static void
imx_fpga_handler(unsigned int mask, unsigned int irq,
                 struct irq_desc *desc)
{
	DEBUG_FPGA_IRQ("%s: mask:0x%x\n", __FUNCTION__, mask);
	desc = irq_desc + irq;
	while (mask) {
		if (mask & 1) {
			DEBUG_FPGA_IRQ("handling irq %d\n", irq);
			desc_handle_irq(irq, desc);
		}
		irq++;
		desc++;
		mask >>= 1;
	}
}

static void
imx_fpga_demux_handler(unsigned int irq_unused, struct irq_desc *desc)
{
	unsigned int mask, irq;

	mask = readw(FPGA_ISR);
	irq = IRQ_FPGA_START;
	imx_fpga_handler(mask, irq, desc);
}

static struct irq_chip imx_fpga_chip = {
	.name = "FPGA",
	.ack = imx_fpga_ack_irq,
	.mask = imx_fpga_mask_irq,
	.unmask = imx_fpga_unmask_irq,
	.set_type = imx_fpga_irq_type,
};


static int __init ocore_irq_mng_init(void)
{
	unsigned int irq;

	DEBUG_FPGA_IRQ("Initializing FPGA IRQs\n");

	/* Mask all interrupts initially */
	writew(0, FPGA_IMR);

	for (irq = IRQ_FPGA(0); irq < IRQ_FPGA(NB_IT-1); irq++) {
		set_irq_chip_and_handler( irq, &imx_fpga_chip, handle_edge_irq );
		set_irq_flags(irq, IRQF_VALID);
	}
	set_irq_chained_handler(APF9328_FPGA_IRQ, imx_fpga_demux_handler);

	DEBUG_FPGA_IRQ("IRQs initialized\n");

	return 0;
}

static void __exit ocore_irq_mng_exit(void)
{
	/* Should we allow to remove this module ?? */
	DEBUG_FPGA_IRQ("%s\n", __FUNCTION__);
}

module_init(ocore_irq_mng_init);
module_exit(ocore_irq_mng_exit);

MODULE_AUTHOR("Julien Boibessot, <julien.boibessot@armadeus.com>");
MODULE_DESCRIPTION("Armadeus OpenCore IRQ manager");
MODULE_LICENSE("GPL");
