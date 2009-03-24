/***********************************
 *  Pulse generator
 *
 *  author : Gwenhael GOAVEC-MEROU
 *
 ***********************************/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <gpio.h>

#define TIMESLEEP HZ/100
#define MINOR_PORT 4

MODULE_AUTHOR("Gwenhael GOAVEC MEROU");
MODULE_DESCRIPTION("sleep kernel test");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");

static int iomask = 0x00;
static struct timer_list mt;

/* timer callback*/
void fonctionTimer(unsigned long arg) {
	imx_gpio_set_value(MINOR_PORT, iomask);
	iomask^=1;
	mt.expires = jiffies+TIMESLEEP;
	add_timer(&mt);
}

/* loading (insmod) */
static int __init blink_init(void) {
	current->state = TASK_INTERRUPTIBLE;
	init_timer(&mt);
	mt.expires = jiffies + TIMESLEEP;
	mt.data = (unsigned long) current;
	mt.function = fonctionTimer;
	add_timer(&mt);
	printk(KERN_INFO "blink_init\n");
	return 0;
}

/* unloading (rmmod) */
static void __init blink_exit(void) {
	printk(KERN_INFO "blink_exit\n");
	del_timer(&mt);
}

/* Api Linux devices */
module_init(blink_init);
module_exit(blink_exit);
