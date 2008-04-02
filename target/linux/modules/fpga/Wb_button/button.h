/* button.h
 * Driver for Wb_button IP
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * 28 mars 2008
 */


#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "button_specific.h"

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
#include <linux/config.h>
#endif

/* form module/drivers */
#include <linux/init.h>
#include <linux/module.h>

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

/* interruptions */
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/wait.h>
#include <asm/semaphore.h>

/* measure time */
#include <linux/jiffies.h>
#endif

/* for debugging messages*/
#define BUTTON_DEBUG

#undef PDEBUG
#ifdef BUTTON_DEBUG
# ifdef __KERNEL__
    /* for kernel spage */
#   define PDEBUG(fmt,args...) printk(KERN_DEBUG "button : " fmt, ##args)
# else
    /* for user space */
#   define PDEBUG(fmt,args...) printk(stderr, fmt, ##args)
# endif
#else
# define PDEBUG(fmt,args...) /* no debbuging message */
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com> - ARMadeus Systems");
MODULE_DESCRIPTION("button device driver");

/* main device structure */
struct button_dev{
  struct cdev cdev;     /* Char device structure */
  struct semaphore sem; /* Mutex */
  void * fpga_virtual_base_address;  /* fpga register base address*/
  wait_queue_head_t button_queue;
  u8 reading;
};

/* major and minor number */

#define N_DEV 1
static dev_t devno; /* to store Major and minor numbers */
static struct button_dev *buttondev;
#define BUTTON_LENGTH 1
struct resource *mem; /* pointer to fpga button */


/* to read write button register */
ssize_t button_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp);

int button_open(struct inode *inode, struct file *filp);

int button_release(struct inode *, struct file *filp);

struct file_operations button_fops = {
  .owner = THIS_MODULE,
  .read  = button_read,
  .open  = button_open,
  .release = button_release,
};

static irqreturn_t  fpga_interrupt(int irq,void *stuff,struct pt_regs *reg);

static void free_all(void);

static ssize_t button_fpga_read(void * addr, u16 *data , struct button_dev *dev);
static ssize_t button_fpga_write(void * addr, u16 *data, struct button_dev *dev);
