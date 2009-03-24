/*
* Small linux kernel driver for handling interrupt 
*
* Copyright (C) 2009 <gwenhael.goavec-merou@armadeus.com>
*                         Armadeus Project / Armadeus Systems
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
#include <asm/irq.h>
#include <linux/irq.h>
#include <asm/signal.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <gpio.h>

#define IRQ_NB IRQ_GPIOA(6)

MODULE_AUTHOR("Gwenhael GOAVEC MEROU");
MODULE_DESCRIPTION("irq kernel test");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");

/* irq handler */
irqreturn_t interrupt_handler (int irqn, void *dev){
	printk("IT !\n");
	return IRQ_HANDLED;
}

/* loading (insmod) */
static int __init irq_init(void) {
	int err;
	printk("irq init\n");
	err =request_irq(IRQ_NB, interrupt_handler, 0,"test irq", NULL);
	if (err!=0){
		printk(KERN_INFO "request_irq : error ");
		switch (err) {
			case -EINVAL:
				printk("EINVAL\n");
				break;
			case -ENOMEM:
				printk("ENOMEM\n");
				break;
			case -EBUSY:
				printk("EBUSY\n");
				break;
			case -ENXIO:
				printk("ENXIO\n");
				break;
		}
		return err;
	}
	set_irq_type( IRQ_NB, IRQF_TRIGGER_FALLING ); 
	return 0;
}

/* unloading (rmmod) */
void __exit irq_exit(void) {
	printk(KERN_INFO "irq_exit\n");
	free_irq(IRQ_NB,NULL);
}

/* Api Linux devices */
module_init(irq_init);
module_exit(irq_exit);
