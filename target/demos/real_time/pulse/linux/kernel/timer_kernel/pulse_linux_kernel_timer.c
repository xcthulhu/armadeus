/*
* Linux kernel driver for generating a signal with timer (GPIO toggling)
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
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/gpio.h>
#include "../../../../common_kernel.h"

MODULE_AUTHOR("Gwenhael GOAVEC MEROU");
MODULE_DESCRIPTION("sleep kernel test");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");

static int iomask = 0x00;
static struct timer_list mt;
#define TS HZ/100
/* timer callback*/
void fonctionTimer(unsigned long arg)
{
	gpio_set_value(PULSE_OUTPUT_GPIO, iomask);
	iomask^=1;
	mt.expires = jiffies+TS;
	add_timer(&mt);
}

/* loading (insmod) */
static int __init blink_init(void)
{
	current->state = TASK_INTERRUPTIBLE;
	init_timer(&mt);
	mt.expires = jiffies + TS;
	mt.data = (unsigned long) current;
	mt.function = fonctionTimer;
	add_timer(&mt);
	
	printk(KERN_INFO "blink_init\n");
	return 0;
}

/* unloading (rmmod) */
static void __exit blink_exit(void)
{
	printk(KERN_INFO "blink_exit\n");
	del_timer(&mt);
}

/* Api Linux devices */
module_init(blink_init);
module_exit(blink_exit);
