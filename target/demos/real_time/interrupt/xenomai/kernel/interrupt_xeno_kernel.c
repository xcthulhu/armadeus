/*
* Small xenomai kernel driver for handling interrupt 
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
#include <native/task.h>
#include <native/timer.h>
#include <rtdm/rtdm_driver.h>
#include <mach/hardware.h>
#include <native/intr.h>


MODULE_LICENSE("GPL");


#define IRQ_NUMBER 70  /* Intercept interrupt on portA6 */
#define TASK_PRIO  99 /* Highest RT priority */
#define TASK_MODE  0  /* No flags */
#define TASK_STKSZ 0  /* Stack size (use default one) */

RT_INTR intr_desc;

static int irq_server (xnintr_t *intr) {
 	printk("IT !\n");
	return RT_INTR_HANDLED;
}

static int __init irq_init(void) {
	int err;
  
	/* Version With 6 param only on kernel space */
	err = rt_intr_create(&intr_desc,"MyIrq", IRQ_NUMBER, irq_server, NULL, 0);	 
	if (err !=0){
		printk("rt_intr_create : error\n");
		return err;
	}
  	err = rt_intr_enable(&intr_desc); 
	if (err != 0)
		printk("rt_intr_enable : error\n");
	else
		printk("rt_intr_create : ok\n");	 
	return err;
}

void __init irq_exit(void) {
	rt_intr_delete(&intr_desc);
}

module_init(irq_init);
module_exit(irq_exit);

