#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <gpio.h>

#define TIMESLEEP 10
#define MINOR_PORT 4

MODULE_AUTHOR("Gwenhael GOAVEC MEROU");
MODULE_DESCRIPTION("sleep kernel test");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE("GPL");

struct task_struct *th;

/* thread fonction */
int fct_thread_sleep(void *data) {
	int iomask=1;
  	do {
    		imx_gpio_set_value(MINOR_PORT, iomask); 
    		iomask^=1;            
    		msleep(TIMESLEEP);
  	}while (!kthread_should_stop());
  	return 0;
}

/* loading (insmod) */
static int __init blink_init(void) {
  	th = kthread_create(fct_thread_sleep,NULL,"pulse_linux_kernel_thread_sleep");
	wake_up_process(th);
	return 0;
}


/* unloading (rmmod) */
static void __init blink_exit(void) {
	printk(KERN_INFO "blink_exit\n");
	kthread_stop(th);
}

/* API Linux devices */
module_init(blink_init);
module_exit(blink_exit);
