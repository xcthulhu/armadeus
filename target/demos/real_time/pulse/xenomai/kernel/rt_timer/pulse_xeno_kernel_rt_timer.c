/*
* Xenomai kernel driver for generating pulses 
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
#include <native/sem.h>
#include <mach/hardware.h>
#include <nucleus/types.h>
#include <gpio.h>

MODULE_LICENSE("GPL");

RT_TASK blink_task;
static int end = 0;

#define TIMESLEEP  10000000llu
#define MINOR_PORT 4
#define GPIOWRDIRECTION _IOW(PP_IOCTL, 0xF1, int)
#define MXC_DR(x)      (0x1c + ((x) << 8))
#define VA_GPIO_BASE       IO_ADDRESS(IMX_GPIO_BASE)
#define PORT_ADDR VA_GPIO_BASE + MXC_DR(0)

void blink(void *arg){
  int err, iomask;
  printk(KERN_INFO "entrering blink\n");
  
  if (imx_gpio_request(GPIO_PORTA | MINOR_PORT, "blink") < 0) {
    gpio_free(MINOR_PORT);
    return ;
  }
  imx_gpio_direction_output(GPIO_PORTA | MINOR_PORT,1);
  
  if ((err = rt_task_set_periodic(NULL, TM_NOW,rt_timer_ns2ticks(TIMESLEEP)))){
    printk("rt task set periodic failed \n");
    return;
  }
  iomask=0;
  
  while(!end){
    rt_task_wait_period(NULL);
    imx_gpio_set_value(GPIO_PORTA | MINOR_PORT, iomask);
    iomask^=1;
  }
  printk("fin\n");
}


static int __init blink_init(void) {
  printk(KERN_INFO "blink_init\n");
  if (rt_task_create(&blink_task, "blink", 0, 99, 0)) {
    printk("task create error\n");
    return -EBUSY;
  }
  if (rt_task_start(&blink_task, &blink, NULL)) { 
    printk("task start error\n"); 
    return -EBUSY; 
  }
  return 0;
}

/* Fin de la tache (rmmod) */
static void __init blink_exit(void) {
  end = 1;
  printk(KERN_INFO "blink_exit\n");
  rt_task_delete(&blink_task);
  rt_timer_stop();
  gpio_free(MINOR_PORT);
}

/* Points d'entrée API modules Linux */
module_init(blink_init);
module_exit(blink_exit);
