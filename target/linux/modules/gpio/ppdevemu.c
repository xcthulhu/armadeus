/*
 * Armadeus Parallel port (ppdev) emulation on APF9328 LCD port
 *
 * Copyright (C) 2006-2009 Julien Boibessot <julien.boibessot@armadeus.com>
 *                         Armadeus Project / Armadeus Systems
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
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
/* Pretend we're PPDEV for IOCTL */
#include <linux/ppdev.h>
#include <asm/io.h>
#include <mach/hardware.h>
#ifdef CONFIG_ARCH_IMX /* TO BE REMOVED WHEN MX1 PLATFORM EXIST */
#include "iomux-mx1.h"
#endif

extern void         gpioWriteOnPort( unsigned int, unsigned int );
extern unsigned int gpioReadFromPort( unsigned int );
extern void         gpioSetPortDir( unsigned int, unsigned int );
extern unsigned int gpioGetPortDir( unsigned int );

#define DRIVER_VERSION            "v0.2"
#define PPDEV_DEVICE_NAME "Armadeus_ppdev"

/* by default, we use dynamic allocation of major numbers */
#define PPDEV_MAJOR 0
#define PPDEV_MAX_MAJOR 254
#define PPDEV_MINOR 0
#define PPDEV_MAX_MINOR 4

#define PORT_A      0
#define PORT_B      1
#define PORT_C      2
#define PORT_D      3
#define PORT_MAX_ID 4

/*#define PORTB27_21MASK    0x0FF00000
#define PORTB27_21SHIFT   20*/
#define PORT_D_31_10_MASK 0xFFFFFC00

struct ppdev_operations *driver_ops;
static int ppdev_major = PPDEV_MAJOR;
/*int __number_of_pins;
static int number_of_pins[4] = {32, 32, 32, 32};*/

static unsigned long init_map;
struct semaphore ppdev_sema;
/* global variables */
/*static unsigned int gPortAIndex = PORT_A;
static unsigned int gPortBIndex = PORT_B;
static unsigned int gPortCIndex = PORT_C;
static unsigned int gPortDIndex = PORT_D;*/

static unsigned char port_mode = 8; /* 8bits mode by default */
module_param( port_mode, byte, 0 );
MODULE_PARM_DESC(port_mode, "Choose between 8 or 4 bits mode");


void __exit armadeus_ppdev_cleanup(void);

/*
 * Low level functions
 */
static void initialize_port( void )
{
	u32 temp;

	/* If not already done by core module */
	temp = __raw_readl(VA_GPIO_BASE + MXC_GIUS(PORT_D)) | PORT_D_31_10_MASK; /* set only portD 31..10 */
	__raw_writel(temp, VA_GPIO_BASE + MXC_GIUS(PORT_D));
	temp = __raw_readl(VA_GPIO_BASE + MXC_PUEN(PORT_D)) | PORT_D_31_10_MASK; /* set only portD 31..10 */
	__raw_writel(temp, VA_GPIO_BASE + MXC_PUEN(PORT_D));
	temp = __raw_readl(VA_GPIO_BASE + MXC_OCR1(PORT_D)) | 0xFFFF0000;
	__raw_writel(temp, VA_GPIO_BASE + MXC_OCR1(PORT_D));
	temp = __raw_readl(VA_GPIO_BASE + MXC_OCR2(PORT_D)) | 0xFFFFFFFF;
	__raw_writel(temp, VA_GPIO_BASE + MXC_OCR2(PORT_D));
}

/* Handles write() done on /dev/ppdevxx */
/*static ssize_t armadeus_ppdev_write(struct file *file, const char *data, size_t count, loff_t *ppos)
{
}*/

/* Handles read() done on /dev/ppdevxx */
/*static ssize_t armadeus_ppdev_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
}*/

/* Handles open() system call done on /dev/... */
static int armadeus_ppdev_open(struct inode *inode, struct file *file)
{
	unsigned m = MINOR(inode->i_rdev);
	if (m != PPDEV_MINOR) {
		printk("Not the right parport minor\n");
		return -EINVAL;
	}

	pr_debug("Opening /dev/ppdev%d file\n", m);
	return 0;
}

/* Handles close() system call done on /dev/... */
static int armadeus_ppdev_release(struct inode *inode, struct file *file)
{
	pr_debug("Closing access to /dev/ppdev\n");
	return 0;
}

/* Handling of some PPDEV IOCTL calls */
int armadeus_ppdev_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
	int err = 0; int ret = 0;
	int value=0;
	unsigned int minor;
	unsigned int lShadow = 0;
	unsigned int PP_DDIR_MASK=0;

	pr_debug(" ## IOCTL received: (0x%x) ##\n", cmd);

	/* Extract the type and number bitfields, and don't decode wrong cmds:
	   return ENOTTY (inappropriate ioctl) before access_ok() */
	if (_IOC_TYPE(cmd) != PP_IOCTL) 
		return -ENOTTY;

	/* The direction is a bitmask, and VERIFY_WRITE catches R/W transfers.
	 `Type' is user-oriented, while access_ok is kernel-oriented,
	  so the concept of "read" and "write" is reversed */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;

	if (down_interruptible(&ppdev_sema))
		return -ERESTARTSYS;

	minor = MINOR(inode->i_rdev);
	if (minor > PPDEV_MAX_MINOR ) {
		printk("Minor outside range: %d !\n", minor);
		return -EFAULT;
	}

	switch (cmd)
	{
		case PPCLAIM:
		case PPRELEASE:
		break;

		case PPFCONTROL: // Normally not used in 4bits mode because in this case control signals are generated with data port
			ret = __get_user(value, (unsigned char *)arg);
			/* Write ctrl infos on assigned ctrl ppdevlines how ??? */
			printk("/FROB 0x%x/ ", value);
			/* Get value from port and clear bits we will set */
			lShadow = gpioReadFromPort(PORT_D);
			/* Control lines are on PortD[14,13,12] */
			lShadow &= 0xFFFF8FFF;
			/* Control are written on iMX LCD Port control in GPIO mode, ie PortD[14,13,12] */
			lShadow |= ((value & 0x0F) << 12);
			/* Put it on port */
			gpioWriteOnPort(PORT_D, lShadow);
			printk(" 0x%x /", lShadow);
		break;

		/* IOCTL used to write on data register of the simulated
		 parallel port. We will ask LCD4LINUX to use 4 bits mode
		 for controlling the LCD and to use the 4 remaining data
		 line as control lines.
		 So, 4 lowest bit received as argument are data and
		 4 highest bits are control
		 !! Minor number is ignored for all PPDEV IOCTLs !! */
		case PPWDATA:
			ret = __get_user(value, (unsigned char *)arg);
			/*printk("/WD 0x%x ", value);*/
			if (ret !=0 )
				break;

			/* Get value from port and clear bits we will set */
			lShadow = gpioReadFromPort(PORT_D);
			if (port_mode == 4) {
				lShadow &= 0xFF000FFF; //0xFFF00FFF;
				/* Control (4 highest bits) are written on iMX LCD
				Port control in GPIO mode, ie PortD[14,13,12] */
				lShadow |= ((value & 0x70) << (12-4));
				/* Data (4 lowest bits) are written on iMX LCD port
				data in GPIO mode, ie PortD[18-15] */
				lShadow |= ((value & 0x0F) << (15+4));
			} else {
				lShadow &= 0xFF807FFF;
				lShadow |= ((value & 0xFF) << 15);
			}
			gpioWriteOnPort(PORT_D, lShadow);
			/*printk(" 0x%x /", lShadow); */
		break;

		case PPRDATA:
			if (port_mode == 4) {
				/* Data (4 lowest bits) are read from iMX LCD port data
				in GPIO mode, ie PortD[18-15] */
				value = (gpioReadFromPort(PORT_D) >> (15+4)) & 0x0F;
			} else {
				value = (gpioReadFromPort(PORT_D) >> 15) & 0xFF;
			}
			
			ret = __put_user(value, (unsigned char *)arg);
			printk("/RD 0x%x/ ", value);
		break;

		case PPDATADIR:
			ret = __get_user(value, (unsigned char *)arg);
			printk("/DIR 0x%x/ ", value);
			
			if (port_mode == 4) {
				PP_DDIR_MASK = 0xFF807FFF; /* 1111 1111 1111 1000 0111 1111 1111 1111 ?? */
			} else {
				PP_DDIR_MASK = 0xFF807FFF; /* 1111 1111 1000 0000 0111 1111 1111 1111 */
			}
			/* linux/ppdev.h define PPDATADIR as "Data line direction: non-zero for input mode."
			For ppdevs, the logic is reversed - bit=1 == output
			This is _not_ "generic" at all, but very much hard-wired towards being able to use an HD44780 LCD on the GPIO pins
			(in 4-bit mode) and being able to do so using generic ppdev instructions
			So, GPIO4-GPIO7 (=ctrl signals) will _always_ be set to output for this call, only GPIO0-GPIO3 (=datalines) are changed */
			if (ret == 0) {
				lShadow = gpioGetPortDir( PORT_D );
				/* only 4 data lines can be set to ouput/input, control lines are always output...*/
				if (value==0) {
					gpioSetPortDir( PORT_D, (lShadow | (!PP_DDIR_MASK)) );
				} else {
					gpioSetPortDir( PORT_D, (lShadow & PP_DDIR_MASK) );
				}
			} else {
				printk(PPDEV_DEVICE_NAME ": ret=%x\n", ret);
			}
		break;

		default:
			return -ENOTTY;
		break;
	}

	up(&ppdev_sema);
	
	return ret;
}

static struct file_operations ppdev_fops = {
	.owner   = THIS_MODULE,
	/*.write   = armadeus_ppdev_write,
	.read    = armadeus_ppdev_read,*/
	.open    = armadeus_ppdev_open,
	.release = armadeus_ppdev_release,
	.ioctl   = armadeus_ppdev_ioctl,
};


int __init armadeus_ppdev_init(void)
{
	static int result;

	printk("Initializing Armadeus Parallel Port emulation driver\n");
	printk("    Port mode: %d bits\n", port_mode );
	init_map = 0;

	/* Configure HW ports/GPIOs with default values or given parameters */
	initialize_port();

	/* Register the driver by getting a major number */
	result = register_chrdev(ppdev_major, PPDEV_DEVICE_NAME, &ppdev_fops);
	if (result < 0) {
		printk(KERN_WARNING PPDEV_DEVICE_NAME ": can't get major %d\n", ppdev_major);
		return result;
	}
	/* Dynamic Major allocation */
	if (ppdev_major == 0)
		ppdev_major = result;

	/* Initialise GPIO port access semaphore */
	sema_init(&ppdev_sema, 1);

	printk(DRIVER_VERSION " successfully loaded !\n");

	return(0);
}

void __exit armadeus_ppdev_cleanup(void)
{
	unregister_chrdev(ppdev_major, PPDEV_DEVICE_NAME);
}


module_init(armadeus_ppdev_init);
module_exit(armadeus_ppdev_cleanup);
MODULE_AUTHOR("JB / NC");
MODULE_DESCRIPTION("Armadeus Parallel Port");
MODULE_LICENSE("GPL");

