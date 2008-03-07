/* Driver for bus_led IP
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * 4 march 2008
 */


#ifndef __LED_H__
#define __LED_H__

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#include <linux/config.h>
#endif

/* form module/drivers */
#include <linux/init.h>
#include <linux/module.h>

/* linux device model */
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/sysdev.h>
#include <linux/device.h>

/* for file  operations */
#include <linux/fs.h>
#include <linux/cdev.h>

/* copy_to_user function */
#include <asm/uaccess.h>

/* request_mem_region */
#include <linux/ioport.h>

/* readw() writew() */
#include <asm/io.h>

/* hardware addresses */
#include <asm/hardware.h>
#endif

/* for debugging messages*/
#define LED_DEBUG

#undef PDEBUG
#ifdef LED_DEBUG
# ifdef __KERNEL__
    /* for kernel spage */
#   define PDEBUG(fmt,args...) printk(KERN_DEBUG "led : " fmt, ##args)
# else
    /* for user space */
#   define PDEBUG(fmt,args...) printk(stderr, fmt, ##args)
# endif
#else
# define PDEBUG(fmt,args...) /* no debbuging message */
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("Led device driver");

/* cdev struct */
struct led_dev{
  struct cdev cdev;     /* Char device structure */
  struct semaphore sem; /* Mutex */
  struct kobject kobj;
};

static void kobj_release(struct kobject *);

#define DRIVER_NAME "led"

/* major and minor number */
#define N_DEV 1
static dev_t devno; /* to store Major and minor numbers */
struct led_dev *sdev;

/* Fpga base address for led */
#define LED_BASE_ADDR 0x12000000
#define LED_LENGTH 1
struct resource *mem; /* pointer to fpga led */


/* to read write led register */
ssize_t led_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp);

ssize_t led_write(struct file *fildes, const char __user *
                  buff,size_t count, loff_t *offp);
int led_open(struct inode *inode, struct file *filp);

int led_release(struct inode *, struct file *filp);

struct file_operations led_fops = {
  .owner = THIS_MODULE,
  .read  = led_read,
  .write = led_write,
  .open  = led_open,
  .release = led_release,
};


/* Sysfs operation */
static struct attribute led_status_attr = {
  .name = "status",
  .owner = THIS_MODULE,
  .mode = 0666,
};
static int init_sysinterface(struct led_dev *pdev);
static ssize_t led_show_status(struct kobject *kobj,struct attribute *attr,char *buf);
static ssize_t led_store_status(struct kobject *kobj,struct attribute *attr, const char *buff, size_t size);

static struct sysfs_ops led_status_fs_ops = {
  .show = led_show_status,
  .store = led_store_status,
};

struct kobj_type kobjtype = {
  .release = kobj_release,
  .sysfs_ops = &led_status_fs_ops,
};


static void free_all(void);
