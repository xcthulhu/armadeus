/*
 * Armadeus i.MXL/27 GPIO management driver
 *
 * Copyright (C) 2006-2008 Julien Boibessot <julien.boibessot@armadeus.com>
 *                         Nicolas Colombain <nicolas.colombain@armadeus.com>
 *                         Armadeus Project / Armadeus Systems
 *
 * Inspired by a lot of other GPIO management systems...
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
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h> /* request_irq */
#include <linux/irq.h>       /* set_irq_type */
#include <asm/gpio.h>        /* imx_gpio_... */
#include <linux/cdev.h>	     /* struct cdev */
#include <mach/hardware.h>
#include <asm/mach/map.h>
#ifdef CONFIG_ARCH_MX2
#include <mach/iomux-mx1-mx2.h>
#endif
#ifdef CONFIG_ARCH_IMX /* TO BE REMOVED WHEN MX1 PLATFORM EXIST */
#include "iomux-mx1.h"
#endif

#include "core.h" /* for ioctl defs */


#define DRIVER_NAME    "Armadeus GPIOs driver"
#define DRIVER_VERSION "2.4"
/* By default, we use dynamic allocation of major numbers -> MAJOR = 0 */
#define GPIO_MAJOR 0
#define GPIO_PROC_DIRNAME         "driver/gpio"

#define GPIO_PROC_FILE 1
#define SETTINGS_PROC_FILE        (1 << 3)
#define SETTINGS_IRQ_PROC_FILE    (1 << 4)
#define SETTINGS_PULLUP_PROC_FILE (1 << 5)
#define SETTINGS_MODE_PROC_FILE   (1 << 6)

#define MAX_NUMBER_OF_PINS 32

#define PORT_A      0
#define PORT_B      1
#define PORT_C      2
#define PORT_D      3
#ifdef CONFIG_ARCH_IMX
#define NB_PORTS    4
#endif
#ifdef CONFIG_ARCH_MX2
#define PORT_E      4
#define PORT_F      5
#define NB_PORTS    6
#endif

#define MAX_MINOR (255) /* Linux limitation */

#define FULL_PORTA_MINOR (MAX_MINOR - PORT_A)
#define FULL_PORTB_MINOR (MAX_MINOR - PORT_B)
#define FULL_PORTC_MINOR (MAX_MINOR - PORT_C)
#define FULL_PORTD_MINOR (MAX_MINOR - PORT_D)
#ifndef CONFIG_ARCH_IMX
#define FULL_PORTE_MINOR (MAX_MINOR - PORT_E)
#define FULL_PORTF_MINOR (MAX_MINOR - PORT_F)
#endif
#define FULL_MINOR_TO_PORT(x) (MAX_MINOR - x)

/* Parameters order: */
enum {
	DDIR_I = 0,
	OCR1_I,
	OCR2_I,
	ICONFA1_I,
	ICONFA2_I,
	ICONFB1_I,
	ICONFB2_I,
	DR_I,
	GIUS_I,
	SSR_I,
	ICR1_I,
	ICR2_I,
	IMR_I,
	GPR_I,
	SWR_I,
	PUEN_I,
};

/* Global variables */
struct gpio_operations *driver_ops;
static int gpio_major = GPIO_MAJOR;

#ifdef CONFIG_ARCH_IMX
static int number_of_pins[NB_PORTS] = {32, 32, 32, 32};
#else
static int number_of_pins[NB_PORTS] = {32, 32, 32, 32, 32, 32};
#endif

static unsigned long init_map;
struct semaphore gpio_sema;

enum {
	VALUE = 0,
	DIRECTION,
	PULL_UP,
	INTERRUPT,
	MODE,
	NB_SETTINGS, /* nb of settings in this enum -> should always be the last ! */
};

struct gpio_settings {
	unsigned int port;
	int type;
};

/* Module parameters */
#define NB_CONFIG_REGS 16
static int portA_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portA_init_nb = 0;
module_param_array( portA_init, int, &portA_init_nb, 0000 );
static int portB_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portB_init_nb = 0;
module_param_array( portB_init, int, &portB_init_nb, 0000 );
static int portC_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portC_init_nb = 0;
module_param_array( portC_init, int, &portC_init_nb, 0000 );
static int portD_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portD_init_nb = 0;
module_param_array( portD_init, int, &portD_init_nb, 0000 );
#ifndef CONFIG_ARCH_IMX
static int portE_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portE_init_nb = 0;
module_param_array( portE_init, int, &portE_init_nb, 0000 );
static int portF_init[NB_CONFIG_REGS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int portF_init_nb = 0;
module_param_array( portF_init, int, &portF_init_nb, 0000 );
#endif

struct gpio_item {
	spinlock_t lock;

	int enabled;
	int initialized;
	int port;
	int nb_pins;
	int number;
	unsigned char irq_value;
	u32 pin_mask;
	u32 oe_mask;

	/* Pin state last time we read it (for blocking reads) */
	u32 pin_state;
	int changed;

	wait_queue_head_t change_wq;
	struct fasync_struct *async_queue;

	int id;
	struct class_device *gpio_dev;
	struct cdev char_dev;
	/* struct config_item item; TBDJUJU: Use configfs filesystem ! */
};

#ifdef CONFIG_ARCH_IMX
static unsigned int shadows_irq_h[NB_PORTS] = { 0, 0, 0, 0 };
static unsigned int shadows_irq_l[NB_PORTS] = { 0, 0, 0, 0 };
#else
static unsigned int shadows_irq_h[NB_PORTS] = { 0, 0, 0, 0, 0, 0 };
static unsigned int shadows_irq_l[NB_PORTS] = { 0, 0, 0, 0, 0, 0 };
#endif


/* Static functions */

static void __exit armadeus_gpio_cleanup(void);

static int toString(unsigned long value, char* buffer, int number_of_bits, int base)
{
	static int i,j;
	char mask = 0x01|base;

	/* convert it into a string */
	for (j=(base*number_of_bits)-(base), i = number_of_bits; i > 0; i--,j-=base) {
		buffer[number_of_bits-i]=((value >> j)&mask)+'0';
	}

	buffer[number_of_bits] = '\n';
	buffer[number_of_bits+1] = 0;

	return number_of_bits+1;
}

/* Convert binary string ("010011") to int. Don't care of non '0' / '1' chars */
static unsigned long fromString(char* buffer, int number_of_bits, int base)
{
	int i, j;
	unsigned long ret_val=0;

	ret_val = 0;
	/* Create WORD to write from the string */
	for (i=0, j=1; j<=number_of_bits; i++) {
		//printk("%x j=%d i=%d\n", buffer[i], j, i);
		if (buffer[i] == '\0')
			break; /* EOC */

		ret_val <<= base;
		ret_val |= (buffer[i]-'0') & 0xff;
		j++;
	}

	return ret_val;
}

/* Return the interrupt config for a pin */
static unsigned char get_irq_from_pin(int num_pin, int num_port)
{
	unsigned long shad;
	int portSize = number_of_pins[num_port];

	if (num_pin < (portSize/2)) {
		shad = shadows_irq_l[num_port];
	} else {
		shad = shadows_irq_h[num_port];
		num_pin -= (portSize/2);
	}

	return shad >> (/*portSize-2-*/(2*num_pin))& 3;
}

/*
 * Low level functions
 */
/* These masks are for restricting user access to configuration of some
   criticals GPIO pins used by Armadeus and not configurable */
#ifdef CONFIG_ARCH_IMX
static unsigned long PORT_MASK[]= { 0x0003FFFE, 0xF00FFF00, 0x0003E1F8, 0xFFFFFFFF };
#else
static unsigned long PORT_MASK[]= { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFC000, 0xFFFE0000, 0x00FFCCF8, 0x00FFFF80 };
#endif

static void initialize_port(int port, int* init_params)
{
	u32 lTemp;

	/* Initialize PORT with module parameters */
	if (init_params[DR_I]) {
		__raw_writel(init_params[DR_I], VA_GPIO_BASE + MXC_DR(port));
	}
	if (init_params[SSR_I]) {
		__raw_writel(init_params[SSR_I], VA_GPIO_BASE + MXC_SSR(port));
	}
	if (init_params[OCR1_I]) {
		__raw_writel(init_params[OCR1_I], VA_GPIO_BASE + MXC_OCR1(port));
	}
	/*else { default value are already set by iMX !!*/
	if (init_params[OCR2_I]) {
		__raw_writel(init_params[OCR2_I], VA_GPIO_BASE + MXC_OCR2(port));
	}
	if (init_params[ICONFA1_I]) {
		__raw_writel(init_params[ICONFA1_I], VA_GPIO_BASE + MXC_ICONFA1(port));
	}
	if (init_params[ICONFA2_I]) {
		__raw_writel(init_params[ICONFA2_I], VA_GPIO_BASE + MXC_ICONFA2(port));
	}
	if (init_params[ICONFB1_I]) {
		__raw_writel(init_params[ICONFB1_I], VA_GPIO_BASE + MXC_ICONFB1(port));
	}
	if (init_params[ICONFB2_I]) {
		__raw_writel(init_params[ICONFB2_I], VA_GPIO_BASE + MXC_ICONFB2(port));
	}
	if (init_params[DDIR_I]) {
		__raw_writel(init_params[DDIR_I], VA_GPIO_BASE + MXC_DDIR(port));
	}
	if (init_params[SWR_I]) {
		__raw_writel(init_params[SWR_I], VA_GPIO_BASE + MXC_SWR(port));
	}
	if (init_params[GPR_I]) {
		lTemp = __raw_readl(VA_GPIO_BASE + MXC_GPR(port)) & (~PORT_MASK[port]);
		__raw_writel(lTemp | (init_params[GPR_I] & PORT_MASK[port]), VA_GPIO_BASE + MXC_GPR(port));
	}
	if (init_params[GIUS_I]) {
		lTemp = __raw_readl(VA_GPIO_BASE + MXC_GIUS(port)) & (~PORT_MASK[port]);
		__raw_writel(lTemp | (init_params[GIUS_I] & PORT_MASK[port]), VA_GPIO_BASE + MXC_GIUS(port));
	}
	if (init_params[PUEN_I]) {
		lTemp = __raw_readl(VA_GPIO_BASE + MXC_PUEN(port)) & (~PORT_MASK[port]);
		__raw_writel(lTemp | (init_params[PUEN_I] & PORT_MASK[port]), VA_GPIO_BASE + MXC_PUEN(port));
	}
	if (init_params[ICR1_I]) {
		__raw_writel(init_params[ICR1_I], VA_GPIO_BASE + MXC_ICR1(port));
	}
	if (init_params[ICR2_I]) {
		__raw_writel(init_params[ICR2_I], VA_GPIO_BASE + MXC_ICR2(port));
	}
	if (init_params[IMR_I]) {
		__raw_writel(init_params[IMR_I], VA_GPIO_BASE + MXC_IMR(port));
	}
}

static void initialize_all_ports(void)
{
	initialize_port(PORT_A, portA_init);
	initialize_port(PORT_B, portB_init);
	initialize_port(PORT_C, portC_init);
	initialize_port(PORT_D, portD_init);
#ifndef CONFIG_ARCH_IMX
	initialize_port(PORT_E, portE_init);
	initialize_port(PORT_F, portF_init);
#endif
}


static void writeOnPort(unsigned int aPort, unsigned int aValue)
{
	__raw_writel(aValue, VA_GPIO_BASE + MXC_DR(aPort));
}

static unsigned int readFromPort(unsigned int aPort)
{
	unsigned int port_value = 0;

	/* Get the status of the gpio ports */
	port_value = __raw_readl(VA_GPIO_BASE + MXC_SSR(aPort));

	return port_value;
}

static void setPortMode(unsigned int aPort, unsigned int aModeMask)
{
	int i;
	int ocr1, ocr2, gius;

	ocr1 = __raw_readl(VA_GPIO_BASE + MXC_OCR1(aPort));
	ocr2 = __raw_readl(VA_GPIO_BASE + MXC_OCR2(aPort));
	gius = __raw_readl(VA_GPIO_BASE + MXC_GIUS(aPort));
	aModeMask = aModeMask & PORT_MASK[aPort]; /* only sets the allowed pins */
	for (i=0; i < number_of_pins[aPort]; i++) {
		if (i < 16) {
			if ((aModeMask>>i) & 1)
				ocr1 = ocr1 | (3<<(i*2));
		} else {
			if ((aModeMask>>i) & 1)
				ocr2 = ocr2 | (3<<((i-16)*2));
		}
	}
	__raw_writel(ocr1, VA_GPIO_BASE + MXC_OCR1(aPort));
	__raw_writel(ocr2, VA_GPIO_BASE + MXC_OCR2(aPort));
	gius = gius | aModeMask;
	__raw_writel(gius, VA_GPIO_BASE + MXC_GIUS(aPort));
}

static unsigned int getPortMode(unsigned int aPort)
{
	int i;
	int ocr1, ocr2, gius, value = 0;

        ocr1 = __raw_readl(VA_GPIO_BASE + MXC_OCR1(aPort));
        ocr2 = __raw_readl(VA_GPIO_BASE + MXC_OCR2(aPort));
	gius = __raw_readl(VA_GPIO_BASE + MXC_GIUS(aPort));
	/* pin is a GPIO if OCR == 3 AND GIUS == 1 */
	for (i=0; i < number_of_pins[aPort]; i++) {
		if (i < 16) {
                        if (((ocr1>>(i*2)) & 3) == 3)
                                value = value | (1<<i);
                } else {
                        if (((ocr2>>((i-16)*2)) & 3) == 3)
				value = value | (1<<i);
                }
	}
	value = value & gius;

	return value;
}

static unsigned int getPortPullUp(unsigned int aPort)
{
        return __raw_readl(VA_GPIO_BASE + MXC_PUEN(aPort));
}

static void setPortPullUp(unsigned int aPort, unsigned int aPullMask)
{
	__raw_writel(aPullMask & 0xffffffff, VA_GPIO_BASE + MXC_PUEN(aPort));
}


static void setPortDir(unsigned int aPort, unsigned int aDirMask)
{
	__raw_writel(aDirMask & 0xffffffff, VA_GPIO_BASE + MXC_DDIR(aPort));
}

static unsigned int getPortDir(unsigned int aPort)
{
	unsigned int port_value = 0;

	/* Get the status of the GPIO direction registers */
	port_value = __raw_readl(VA_GPIO_BASE + MXC_DDIR(aPort));

	return port_value;
}


char* port_name[NB_PORTS] = { "portA", "portB", "portC", "portD",
#ifdef CONFIG_ARCH_MX2
				"portE", "portF",
#endif
};
char* port_setting_name[NB_SETTINGS] = { "Value", "Direction", "Pull-up", "Interrupt" , "Mode" };


/* Handles write() done on /dev/gpioxx */
static ssize_t armadeus_gpio_dev_write(struct file *file, const char *data, size_t count, loff_t *offset)
{
	unsigned int minor=0;
	u32 value=0;
	ssize_t ret = 0;
	struct gpio_item *gpio = file->private_data;

	minor = MINOR(file->f_dentry->d_inode->i_rdev);

	pr_debug("- %s %d byte(s) on minor %d -> %s pin %d\n", __FUNCTION__,
			count, minor, port_name[gpio->port], gpio->number);

	if (down_interruptible(&gpio_sema)) /* Usefull ?? */
		return -ERESTARTSYS;

	count = min(count, (size_t)4);
	if (copy_from_user(&value, data, count)) {
		ret = -EFAULT;
		goto out;
	}

	if (gpio->nb_pins != 1) {
		pr_debug("Full port write: 0x%x\n", value);
		writeOnPort(gpio->port, value);
	} else {
		value = value ? 1 : 0;
		pr_debug("Single pin write: %d\n", value);
		gpio_set_value(minor, value);
	}
	ret = count;

out:
	up(&gpio_sema);
	return ret;
}

/* Handles read() done on /dev/gpioxx */
static ssize_t armadeus_gpio_dev_read(struct file *file, char *buf,
	size_t count, loff_t *ppos)
{
	unsigned minor = MINOR(file->f_dentry->d_inode->i_rdev);
	u32 value=0;
	ssize_t ret = 0;
	u32 port_status;
	struct gpio_item *gpio = file->private_data;

	if (count == 0)
		return count;

	spin_lock_irq(&gpio->lock);

	pr_debug("- %s %d byte(s) on minor %d -> %s pin %d\n", __FUNCTION__,
		 count, minor, port_name[gpio->port], gpio->number);

	while (!gpio->changed) {
		spin_unlock_irq(&gpio->lock);

		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(gpio->change_wq, gpio->changed))
			return -ERESTARTSYS;

		spin_lock_irq(&gpio->lock);
	}
	gpio->changed = 0;

	if (gpio->nb_pins != 1) {
		value = readFromPort(gpio->port);
		pr_debug("Full port read: 0x%x\n", value);
	} else {
		value = gpio_get_value(minor);
		pr_debug("Single pin read: %d\n", value);
	}

	value = readFromPort(minor);
	port_status = (char)(value & 0xFF);

	count = min(count, (size_t)sizeof(u32));
	if (copy_to_user(buf, &value, count)) {
		ret = -EFAULT;
		goto out;
	}
	ret = count;

out:
	spin_unlock_irq(&gpio->lock);
	return ret;
}


static irqreturn_t armadeus_gpio_interrupt(int irq, void *dev_id)
{
	struct gpio_item *gpio = dev_id;
	u32 old_state, new_state;

	pr_debug("IT for pin %d %d\n", gpio->port, gpio->number);

	old_state = gpio->pin_state;
	new_state = gpio_get_value((gpio->port << GPIO_PORT_SHIFT) | gpio->number);
	gpio->pin_state = new_state;

	if ((gpio->irq_value != (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)) || new_state != old_state) {
		gpio->changed = 1;
		wake_up_interruptible(&gpio->change_wq);

		if (gpio->async_queue)
			kill_fasync(&gpio->async_queue, SIGIO, POLL_IN);
		if (gpio->irq_value == (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)) {
			set_irq_type(irq, ((gpio->pin_state) ? IRQF_TRIGGER_FALLING : IRQF_TRIGGER_RISING));
		}
	}

	return IRQ_HANDLED;
}

/* Handles open() done on /dev/gpioxx */
static int armadeus_gpio_dev_open(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	/*unsigned int major = MAJOR(inode->i_rdev);*/
	unsigned int irq;
	int ret = 0;
	struct gpio_item *gpio;/* = container_of(inode->i_cdev, struct gpio_item,
								char_dev);*/

	gpio = kzalloc(sizeof(struct gpio_item), GFP_KERNEL);
	if (!gpio)
		goto err_request;

	file->private_data = gpio;
	spin_lock_init(&gpio->lock);
	init_waitqueue_head(&gpio->change_wq);

	switch (minor) {
		/* Write all port pins in one time */
		case FULL_PORTA_MINOR:
		case FULL_PORTB_MINOR:
		case FULL_PORTC_MINOR:
		case FULL_PORTD_MINOR:
#ifndef CONFIG_ARCH_IMX
		case FULL_PORTE_MINOR:
		case FULL_PORTF_MINOR:
#endif
			gpio->nb_pins = 32;
			gpio->changed = 1;
			gpio->port = FULL_MINOR_TO_PORT(minor);
			pr_debug("Reserving full %s\n", port_name[gpio->port]);
			goto success;
		break;
	}

	/* Pin by pin access */
	gpio->nb_pins = 1;
	gpio->changed = 1;

	if ((ret = gpio_request(minor, "gpio-dev"))) {
		printk("GPIO %i is already in use\n", minor);
		goto err_request;
	}

	gpio->port = minor >> GPIO_PORT_SHIFT;
	gpio->number = minor & GPIO_PIN_MASK;
	gpio->pin_state = 0;

	if (getPortDir(gpio->port) & (1 << gpio->number)) {
		gpio_direction_output(minor, 0);
	} else {
		gpio_direction_input(minor);
		gpio->pin_state = gpio_get_value(minor);
	}

	/* Request interrupt if pin was configured for */
	gpio->irq_value = get_irq_from_pin(gpio->number, gpio->port);

	if (gpio->irq_value) {
		irq = IRQ_GPIOA(minor); /* irq number are continuous */
		ret = request_irq(irq, armadeus_gpio_interrupt, 0, "gpio", gpio);
		if (ret)
			goto err_irq;
		switch (gpio->irq_value) {
			case (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING):
				;
			case IRQF_TRIGGER_RISING:
				set_irq_type(irq, IRQF_TRIGGER_RISING);
				break;
			case IRQF_TRIGGER_FALLING:
				set_irq_type(irq, IRQF_TRIGGER_FALLING);
				break;
			case IRQF_TRIGGER_NONE:
				break;
		}
	}

success:
	pr_debug("Opening /dev node for %s pin %d\n", port_name[gpio->port], gpio->number);
	gpio->initialized = 1;
	return 0;

err_irq:
	printk("%s error while requesting irq %d\n", __FUNCTION__, irq);
	free_irq(irq, gpio);
err_request:
	kfree(gpio);
	/* what about spinlock & wait_queue ?? */
	return ret;
}

/* Handles close() done on /dev/gpioxx */
static int armadeus_gpio_dev_release(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	struct gpio_item *gpio = file->private_data;
	unsigned int irq;

	if (gpio->initialized) {
		if (gpio->irq_value) {
			irq = IRQ_GPIOA(minor);
			free_irq(irq, gpio);
		}
		gpio_free(minor);
	}
	kfree(gpio);
	pr_debug("Closing access to /dev/gpio/ minor %d\n", minor);

	return 0;
}

/* Handling of IOCTL calls */
int armadeus_gpio_dev_ioctl(struct inode *inode, struct file *filp,
	unsigned int cmd, unsigned long arg)
{
	int err = 0, ret = 0, value = 0;
	unsigned int minor;

	pr_debug(DRIVER_NAME " ## IOCTL received: (0x%x) ##\n", cmd);

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

	/* Obtain exclusive access */
	if (down_interruptible(&gpio_sema))
		return -ERESTARTSYS;

	/* Extract and test minor */
	minor = MINOR(inode->i_rdev);
#ifndef CONFIG_ARCH_IMX
	if (minor < FULL_PORTF_MINOR) {
#else
	if (minor < FULL_PORTD_MINOR) {
#endif
		printk("IOCTLs are only available on 'full port' minors !\n");
		ret = -EFAULT;
		goto out;
	}

	switch (cmd) {
		case GPIORDDIRECTION:
		value = getPortDir(MAX_MINOR - minor);
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIOWRDIRECTION:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			setPortDir(MAX_MINOR - minor, value);
		}
		break;

		case GPIORDDATA:
		value = readFromPort(MAX_MINOR - minor);
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIOWRDATA:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			writeOnPort(MAX_MINOR - minor, value);
		}
		break;

		case GPIORDMODE:
		value = getPortMode(MAX_MINOR - minor);
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIOWRMODE:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			setPortMode(MAX_MINOR - minor, value);
		}
		break;

		case GPIORDIRQMODE_H:
		value  = shadows_irq_h[MAX_MINOR - minor];
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIORDIRQMODE_L:
		value = shadows_irq_l[MAX_MINOR - minor];
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIOWRIRQMODE_H:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			shadows_irq_h[MAX_MINOR - minor] = value;
		}
		break;

		case GPIOWRIRQMODE_L:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			shadows_irq_l[MAX_MINOR - minor] = value;
		}
		break;

		case GPIORDPULLUP:
		value = getPortPullUp(MAX_MINOR - minor);
		ret = __put_user(value, (unsigned int *)arg);
		break;

		case GPIOWRPULLUP:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			setPortPullUp(MAX_MINOR - minor, value);
		}
		break;

		default:
		printk("IOCTL not supported\n");
		ret = -ENOTTY;
		break;
	}
out:
	/* Release exclusive access */
	up(&gpio_sema);

	return ret;
}

static int armadeus_gpio_fasync(int fd, struct file* filp, int on)
{
	struct gpio_item* gpio = filp->private_data;

	return fasync_helper(fd, filp, on, &(gpio->async_queue));
}

static struct file_operations gpio_fops = {
	.owner   = THIS_MODULE,
	.llseek  = no_llseek,
	.write   = armadeus_gpio_dev_write,
	.read    = armadeus_gpio_dev_read,
	.open    = armadeus_gpio_dev_open,
	.release = armadeus_gpio_dev_release,
	.ioctl   = armadeus_gpio_dev_ioctl,
	.fasync  = armadeus_gpio_fasync,
};


/*
 * PROC file functions
 */

static int armadeus_gpio_proc_read(char *buffer, char **start, off_t offset,
	int buffer_length, int *eof, __attribute__ ((unused)) void *data)
{
	int len = 0; /* The number of bytes actually used */
	unsigned int port_status = 0x66, port_status2 = 0x66;
	unsigned int port_ID = 0;
	struct gpio_settings *settings = (struct gpio_settings *) data;

	if (settings != NULL) {
		port_ID = settings->port;
	}

	/* We give all of our information in one go, so if the user asks us if we
	   have more information the answer should always be no. This is important
	   because the standard read function from the library would continue to
	   issue the read system call until the kernel replies that it has no more
	   information, or until its buffer is filled
	*/
	if (offset > 0 || buffer_length < (number_of_pins[port_ID]+2))
		return 0;

	if (down_interruptible(&gpio_sema))
		return -ERESTARTSYS;

	switch (settings->type) {
		case MODE:
			port_status = getPortMode(port_ID);
		break;

		case VALUE:
			port_status = readFromPort(port_ID);
		break;

		case DIRECTION:
			port_status = getPortDir(port_ID);
		break;

		case PULL_UP:
			port_status = __raw_readl(VA_GPIO_BASE + MXC_PUEN(port_ID));
		break;

		case INTERRUPT:
			port_status = shadows_irq_h[port_ID];
			port_status2 = shadows_irq_l[port_ID];
		break;

		default:
			printk("%s: unknown setting\n", __FUNCTION__);
		break;
	}
	/* Put result to given chr buffer */
	if (settings->type != INTERRUPT) {
		len = toString(port_status, buffer, number_of_pins[port_ID], 1);
	} else {
		len = toString(port_status, buffer, number_of_pins[port_ID]/2, 2);
		len += toString(port_status2, buffer+len-1, number_of_pins[port_ID]/2, 2);
		len -= 1;
	}

	pr_debug("%s: 0x%08x", __func__, port_status);
	if (settings->type == INTERRUPT)
		pr_debug(" 0x%08x", port_status2);
	pr_debug("\n");

	*eof = 1;
	up(&gpio_sema);

	/* Return the length */
	return len;
}

static char new_gpio_state[MAX_NUMBER_OF_PINS*2];

static int armadeus_gpio_proc_write(__attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, __attribute__ ((unused)) void* data)
{
	int len;
	unsigned int  gpio_state=0, gpio_state2=0;
	unsigned int port_ID = 0;
	struct gpio_settings *settings = (struct gpio_settings *) data;

	if (settings != NULL) {
		port_ID = settings->port;
	}

	/* Do some checks on parameters */
	if (count <= 0) {
		printk("Empty string transmitted !\n");
		return 0;
	}
	if (count > (MAX_NUMBER_OF_PINS + 1))
		printk("GPIO port registers are only 32bits !\n");
	if (count > (sizeof(new_gpio_state))) {
		len = sizeof(new_gpio_state);
	} else {
		len = count;
	}

	/* Get exclusive access to port */
	if (down_interruptible(&gpio_sema))
		return -ERESTARTSYS;

	/* Get datas to write from user space */
	if (copy_from_user(new_gpio_state, buf, len)) {
		up(&gpio_sema);
		return -EFAULT;
	}

	if (strlen(new_gpio_state) > 0) {
		/* Convert it from String to Int */
		if (settings->type != INTERRUPT) {
			gpio_state = fromString(new_gpio_state, number_of_pins[port_ID], 1);
		} else {
			gpio_state =  fromString(new_gpio_state, number_of_pins[port_ID]/2, 2);
			gpio_state2 = fromString(new_gpio_state+(number_of_pins[port_ID]/2), (number_of_pins[port_ID]/2), 2);
		}

		switch (settings->type)
		{
			case MODE:
				setPortMode(port_ID, gpio_state);
			break;

			case VALUE:
				writeOnPort(port_ID, gpio_state);
			break;

			case DIRECTION:
				setPortDir(port_ID, gpio_state);
			break;

			case PULL_UP:
				setPortPullUp(port_ID, gpio_state);
			break;

			case INTERRUPT:
				shadows_irq_h[port_ID] = gpio_state;
				shadows_irq_l[port_ID] = gpio_state2;
			break;

			default:
				printk("%s: unknown setting\n", __FUNCTION__);
			break;
		}

		if (settings->type != INTERRUPT) {
			pr_debug("/proc wrote 0x%x", gpio_state);
		} else {
			pr_debug("/proc wrote 0x%x 0x%x", gpio_state, gpio_state2);
		}
		pr_debug(" on %s %s register\n", port_name[port_ID],
			port_setting_name[settings->type]);
	}

	up(&gpio_sema);
	/* Makes as if we take all the data sent even if we can't handle more
	than register size */
	return count;
}


/* /proc registering helpers */
struct proc_config_entry {
	struct proc_dir_entry *entry;
	char name[32];
	int port;
	int type;
};

static int initialize_proc_entry( struct proc_config_entry *config )
{
	int i;

	for (i = 0; i < NB_PORTS; i++) {
		config[i].entry = create_proc_entry(config[i].name, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
		if (config[i].entry == NULL) {
			printk(KERN_ERR DRIVER_NAME ": Couldn't register %s. Terminating.\n", config[i].name);
			return -ENOMEM;
		}
		config[i].entry->read_proc  = armadeus_gpio_proc_read;
		config[i].entry->write_proc = armadeus_gpio_proc_write;
		config[i].entry->data       = kmalloc(sizeof(struct gpio_settings), GFP_KERNEL);
		((struct gpio_settings*) config[i].entry->data)->port = i;
		((struct gpio_settings*) config[i].entry->data)->type = config[i].type;
	}

	return 0;
}

/*
 * Create /proc entries for direct access (with echo/cat) to GPIOs config
 */
static int create_proc_entries(void)
{
	struct proc_config_entry proc_config[NB_PORTS];
	int ret, i;

	pr_debug("Creating /proc entries: ");

	/* Create main directory */
	proc_mkdir(GPIO_PROC_DIRNAME, NULL);

	/* Create proc file to handle GPIO values */
	for (i = 0; i < NB_PORTS; i++) {
		sprintf(proc_config[i].name, "%s/%s", GPIO_PROC_DIRNAME, port_name[i]);
		proc_config[i].type = VALUE;
	}

	if ((ret = initialize_proc_entry(proc_config)))
		return ret;
	init_map |= GPIO_PROC_FILE;

	/* Create proc file to handle GPIO mode */
	for (i = 0; i < NB_PORTS; i++) {
		sprintf(proc_config[i].name, "%s/%smode", GPIO_PROC_DIRNAME, port_name[i]);
		proc_config[i].type = MODE;
	}

	if ((ret = initialize_proc_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_MODE_PROC_FILE;

	/* Create proc file to handle GPIO direction settings */
	for (i = 0; i < NB_PORTS; i++) {
		sprintf(proc_config[i].name, "%s/%sdir", GPIO_PROC_DIRNAME, port_name[i]);
		proc_config[i].type = DIRECTION;
	}

	if ((ret = initialize_proc_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_PROC_FILE;

	/* Create proc file to handle GPIO interrupt settings */
	for (i = 0; i < NB_PORTS; i++) {
		sprintf(proc_config[i].name, "%s/%sirq", GPIO_PROC_DIRNAME, port_name[i]);
		proc_config[i].type = INTERRUPT;
	}

	if ((ret = initialize_proc_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_IRQ_PROC_FILE;

	/* Create proc file to handle GPIO pullup settings */
	for (i = 0; i < NB_PORTS; i++) {
		sprintf(proc_config[i].name, "%s/%spullup", GPIO_PROC_DIRNAME, port_name[i]);
		proc_config[i].type = PULL_UP;
	}

	if ((ret = initialize_proc_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_PULLUP_PROC_FILE;

	pr_debug("OK!\n");

	return 0;
}

static void remove_proc_entries(void)
{
	char proc_name[32];
	int i;

	/* Remove /proc entries */
	if (init_map & GPIO_PROC_FILE) {
		printk(DRIVER_NAME " removing /proc/.../portX\n");
		for (i = 0; i < NB_PORTS; i++) {
			sprintf(proc_name, "%s/%s", GPIO_PROC_DIRNAME, port_name[i]);
			remove_proc_entry(proc_name, NULL);
		}
	}
	if (init_map & SETTINGS_MODE_PROC_FILE) {
		printk(DRIVER_NAME " removing /proc/.../portXmode\n");
		for (i = 0; i < NB_PORTS; i++) {
			sprintf(proc_name, "%s/%smode", GPIO_PROC_DIRNAME, port_name[i]);
			remove_proc_entry(proc_name, NULL);
		}
	}
	if (init_map & SETTINGS_PROC_FILE) {
		printk(DRIVER_NAME " removing /proc/.../portXdir\n");
		for (i = 0; i < NB_PORTS; i++) {
			sprintf(proc_name, "%s/%sdir", GPIO_PROC_DIRNAME, port_name[i]);
			remove_proc_entry(proc_name, NULL);
		}
	}
	if (init_map & SETTINGS_IRQ_PROC_FILE) {
		printk(DRIVER_NAME " removing /proc/.../portXirq\n");
		for (i = 0; i < NB_PORTS; i++) {
			sprintf(proc_name, "%s/%sirq", GPIO_PROC_DIRNAME, port_name[i]);
			remove_proc_entry(proc_name, NULL);
		}
	}
	if (init_map & SETTINGS_PULLUP_PROC_FILE) {
		printk(DRIVER_NAME " removing /proc/.../portXpullup\n");
		for (i = 0; i < NB_PORTS; i++) {
			sprintf(proc_name, "%s/%spullup", GPIO_PROC_DIRNAME, port_name[i]);
			remove_proc_entry(proc_name, NULL);
		}
	}

	remove_proc_entry(GPIO_PROC_DIRNAME, NULL);
}

static void print_port_params(int port, int nb, int* params)
{
	int i;

	printk("    %s Parameters (%i): ", port_name[port], nb);
	for (i = 0; i < NB_CONFIG_REGS; i++) {
		printk(" 0x%x", params[i]);
	}
	printk("\n");
}

/* Module's initialization */
static int __init armadeus_gpio_init(void)
{
	static int result;

	printk("Initializing Armadeus GPIOs driver\n");
	print_port_params(PORT_A, portA_init_nb, portA_init);
	print_port_params(PORT_B, portB_init_nb, portB_init);
	print_port_params(PORT_C, portC_init_nb, portC_init);
	print_port_params(PORT_D, portD_init_nb, portD_init);
#ifdef CONFIG_ARCH_MX2
	print_port_params(PORT_E, portE_init_nb, portE_init);
	print_port_params(PORT_F, portF_init_nb, portF_init);
#endif
	init_map = 0;

	/* Configure HW ports/GPIOs with default values or given parameters */
	initialize_all_ports();

	/* Register the driver as character device by getting a major number */
	result = register_chrdev(gpio_major, DRIVER_NAME, &gpio_fops);
	if (result < 0) {
		printk(KERN_WARNING DRIVER_NAME ": can't get major %d\n", gpio_major);
		return result;
	}
	if (gpio_major == 0) gpio_major = result; /* dynamic Major allocation */

	/* Creates /proc entries */
	if ((result = create_proc_entries())) {
		remove_proc_entries();
		return result;
	}

	/* Initialise GPIO port access semaphore */
	sema_init(&gpio_sema, 1);

	/* Set GPIOs to initial state: iMX and parameters will do it */

	printk(DRIVER_NAME " v" DRIVER_VERSION " successfully loaded !\n");
	return 0;
}

/* Module's cleanup */
static void __exit armadeus_gpio_cleanup(void)
{
	/* TBDJUJU free config[i].entry->data first ?? */
	remove_proc_entries();

	/* De-register from /dev interface */
	unregister_chrdev(gpio_major, DRIVER_NAME);
}

/*
 * API To be removed ???
 */
void gpioWriteOnPort(unsigned int aPort, unsigned int aValue)
{
	if (aPort >= NB_PORTS) {
		printk(DRIVER_NAME "port unknown !\n");
		return;
	}
	writeOnPort(aPort, aValue);
}

unsigned int gpioReadFromPort(unsigned int aPort)
{
	if (aPort >= NB_PORTS) {
		printk(DRIVER_NAME "port unknown !\n");
		return 0;
	} else {
		return readFromPort(aPort);
	}
}

void gpioSetPortDir(unsigned int aPort, unsigned int aDirMask)
{
	setPortDir(aPort, aDirMask);
}

unsigned int gpioGetPortDir(unsigned int aPort)
{
	return getPortDir(aPort);
}

EXPORT_SYMBOL(gpioWriteOnPort);
EXPORT_SYMBOL(gpioReadFromPort);
EXPORT_SYMBOL(gpioSetPortDir);
EXPORT_SYMBOL(gpioGetPortDir);


module_init(armadeus_gpio_init);
module_exit(armadeus_gpio_cleanup);
MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain");
MODULE_DESCRIPTION("Armadeus GPIOs driver");
MODULE_LICENSE("GPL");

