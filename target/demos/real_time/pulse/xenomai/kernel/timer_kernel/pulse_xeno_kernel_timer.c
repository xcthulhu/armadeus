/*
* Xenomai kernel driver for generating pulse with timer 
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
#include <gpio.h>

MODULE_LICENSE("GPL");

static rtdm_task_t blink_task;
static int end = 0;

#define TIMESLEEP 5000000
#define MINOR_PORT 4
#define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)
#define MXC_DR(x)      (0x1c + ((x) << 8))
#define VA_GPIO_BASE       IO_ADDRESS(IMX_GPIO_BASE)
#define PORT_ADDR VA_GPIO_BASE + MXC_DR(0)

void blink(void *arg){
	int iomask; 
	printk(KERN_INFO "entrering blink\n");
	iomask=0;
	while(!end){
		rtdm_task_wait_period();
		gpio_set_value(MINOR_PORT, iomask);
		iomask^=1;
	}
	printk("end\n");
}

/* module load (insmod) */
static int __init blink_init(void) {
	printk(KERN_INFO "blink_init\n");
	if (gpio_request(MINOR_PORT, "blink") < 0) {
		gpio_free(MINOR_PORT);
		return -EBUSY;
	}
	gpio_direction_output(MINOR_PORT,1);
  	return rtdm_task_init(&blink_task, "blink", blink, NULL,
			99, TIMESLEEP);
}

/* module unload (rmmod) */
static void __init blink_exit(void) {
	end = 1;
	printk(KERN_INFO "blink_exit\n");
	rtdm_task_join_nrt(&blink_task,100);
	gpio_free(MINOR_PORT);
}

/* API kernel driver */
module_init(blink_init);
module_exit(blink_exit);
