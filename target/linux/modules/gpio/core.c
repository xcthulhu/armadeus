/*
 * Armadeus i.MXL GPIO management driver
 *
 * Copyright (C) 2006-2008 Armadeus Project / Armadeus Systems
 * Authors: Nicolas Colombain / Julien Boibessot
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

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#include <asm/arch/imx-regs.h>
#else
#include <mach/imx-regs.h>
#endif

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include "core.h" // TBDJUJU: put core.h contents here....


#define GPIO_PROC_FILE 1
#define SETTINGS_PROC_FILE        (1 << 3)
#define SETTINGS_IRQ_PROC_FILE    (1 << 4)
#define SETTINGS_PULLUP_PROC_FILE (1 << 5)

#define PORT_A      0
#define PORT_B      1
#define PORT_C      2
#define PORT_D      3
#define NB_PORTS    4
#define PORT_MAX_ID 4

#define MAX_MINOR (255) /* Linux limitation */

#define FULL_PORTA_MINOR (MAX_MINOR - PORT_A)
#define FULL_PORTB_MINOR (MAX_MINOR - PORT_B)
#define FULL_PORTC_MINOR (MAX_MINOR - PORT_C)
#define FULL_PORTD_MINOR (MAX_MINOR - PORT_D)

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
static int gpio_major =  GPIO_MAJOR;

static int number_of_pins[4] = {32, 32, 32, 32};

static unsigned long init_map;
struct semaphore gpio_sema;

enum {
	VALUE = 0,
	DIRECTION,
	PULL_UP,
	INTERRUPT,
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


struct gpio_item {
	spinlock_t lock;

	int enabled;
	int initialized;
	int port;
	int nb_pins;
	int number;
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

static unsigned int shadows_irq[NB_PORTS] = { 0, 0, 0, 0 };


/* Static functions */

static void __exit armadeus_gpio_cleanup(void);

static int toString(unsigned long value, char* buffer, int number_of_bits) 
{
	static int i;
	
	/* convert it into a string */
	for (i=number_of_bits;i>0;i--) {
		buffer[number_of_bits-i]=test_bit(i-1,&value)?'1':'0';
	}
	
	buffer[number_of_bits] = '\n';
	buffer[number_of_bits+1] = 0;
	
	return number_of_bits+1;
}

/* Convert binary string ("010011") to int. Don't care of non '0' / '1' chars */
static unsigned long fromString(char* buffer, int number_of_bits) 
{
	int i, j;
	unsigned long ret_val=0;

	ret_val = 0;
	/* Create WORD to write from the string */
	for ( i=0, j=1; j<=number_of_bits; i++) {
		//printk("%x j=%d i=%d\n", buffer[i], j, i);
		if (buffer[i] == '\0') break; /* EOC */
	
		if (buffer[i] == '0' || buffer[i] == '1') {
			ret_val <<= 1;
			if (buffer[i] == '1') {
				ret_val |= 1;
			}
			j++;
		}
	}

	return ret_val;
}

/*
 * Low level functions
 */
#define DEFAULT_VALUE 0x12345678
/* These masks are for restricting user access to configuration of some
   criticals GPIO pins used by Armadeus and not configurable */
static unsigned long MASK[]= { 0x0003FFFE, 0xF00FFF00, 0x0003E1F8, 0xFFFFFFFF };
#define PORT_A_MASK    0x0003FFFE
#define PORT_B_MASK    0xF00FFF00
#define PORT_C_MASK    0x0003E1F8
#define PORT_D_MASK    0xFFFFFFFF

static void initialize_port(int port, int* init_params)
{
	unsigned long lTemp;
	
	/* Initialize PORT with module parameters */
	if (init_params[DR_I]) {      DR(port)      = init_params[DR_I]; }
	if (init_params[SSR_I]) {     SSR(port)     = init_params[SSR_I]; }
	if (init_params[OCR1_I]) {    OCR1(port)    = init_params[OCR1_I]; }
	/*else { default value are already set by iMX !!*/
	if (init_params[OCR2_I]) {    OCR2(port)    = init_params[OCR2_I]; }
	if (init_params[ICONFA1_I]) { ICONFA1(port) = init_params[ICONFA1_I]; }
	if (init_params[ICONFA2_I]) { ICONFA2(port) = init_params[ICONFA2_I]; }
	if (init_params[ICONFB1_I]) { ICONFB1(port) = init_params[ICONFB1_I]; }
	if (init_params[ICONFB2_I]) { ICONFB2(port) = init_params[ICONFB2_I]; }
	if (init_params[DDIR_I]) {    DDIR(port)    = init_params[DDIR_I]; }
	if (init_params[SWR_I]) {     SWR(port)     = init_params[SWR_I]; }
	if (init_params[GPR_I]) {  lTemp = GPR(port)  & (~MASK[port]);  GPR(port)  = lTemp | (init_params[GPR_I]  & MASK[port]); }
	if (init_params[GIUS_I]) { lTemp = GIUS(port) & (~MASK[port]);  GIUS(port) = lTemp | (init_params[GIUS_I] & MASK[port]); }
	if (init_params[PUEN_I]) { lTemp = PUEN(port) & (~MASK[port]);  PUEN(port) = lTemp | (init_params[PUEN_I] & MASK[port]); }
	if (init_params[ICR1_I]) {    ICR1(port)    = init_params[ICR1_I]; }
	if (init_params[ICR2_I]) {    ICR2(port)    = init_params[ICR2_I]; }
	if (init_params[IMR_I]) {     IMR(port)     = init_params[IMR_I]; }
}

static void initialize_all_ports( void )
{
	initialize_port(PORT_A, portA_init);
	initialize_port(PORT_B, portB_init);
	initialize_port(PORT_C, portC_init);
	initialize_port(PORT_D, portD_init);
}


/* TBDJUJU: replace the following functions with imx_gpio_xxx one !! */

static void writeOnPort( unsigned int aPort, unsigned int aValue )
{
	DR(aPort) = (aValue & MASK[aPort]);
}

static unsigned int readFromPort( unsigned int aPort )
{
	unsigned int port_value = 0;

	/* Get the status of the gpio ports */
	port_value = (SSR(aPort));

	return( port_value );
}

static void setPortDir( unsigned int aPort, unsigned int aDirMask )
{
	DDIR(aPort) = (aDirMask & 0xffffffff);
}

static unsigned int getPortDir( unsigned int aPort )
{
	unsigned int port_value = 0;
	
	/* Get the status of the gpio direction registers TBDNICO */
	port_value = (DDIR(aPort));
	
	return( port_value );
}

char* port_name[NB_PORTS]  = { "PortA", "PortB", "PortC", "PortD" };
char* port_setting_name[4] = { "Value", "Direction", "Pull-up", "Interrupt" };


/* Handles write() done on /dev/gpioxx */
static ssize_t armadeus_gpio_dev_write(struct file *file, const char *data, size_t count, loff_t *offset)
{
	unsigned int minor=0;
	u32 value=0;
	ssize_t ret = 0;
	struct gpio_item *gpio = file->private_data;

	minor = MINOR(file->f_dentry->d_inode->i_rdev);

	pr_debug("- %s %d byte(s) on minor %d -> %s pin %d\n", __FUNCTION__, count, minor, port_name[gpio->port], gpio->number);

	if (down_interruptible(&gpio_sema)) /* Usefull ?? */
        	return -ERESTARTSYS;

	count = min(count, (size_t)4);
	if (copy_from_user(&value, data, count)) {
		ret = -EFAULT;
		goto out;
	}

	if( gpio->nb_pins != 1) {
		pr_debug("Full port write: 0x%x\n", value);
		writeOnPort( gpio->port, value );
	} else {
		value = value ? 1 : 0;
		pr_debug("Single pin write: %d\n", value);
		imx_gpio_set_value( minor, value );
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
		value = readFromPort( gpio->port );
		pr_debug("Full port read: 0x%x\n", value);
	} else {
		value = imx_gpio_get_value( minor );
		pr_debug("Single pin read: %d\n", value);
	}
	
	value = readFromPort( minor );
	port_status = (char)(value & 0xFF);

	count = min( count, (size_t)sizeof(u32) );
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

	printk("IT for pin %d %d\n", gpio->port, gpio->number);

	old_state = gpio->pin_state;
	new_state = imx_gpio_get_value( gpio->port|gpio->number );
	gpio->pin_state = new_state;

	if (new_state != old_state) {
		gpio->changed = 1;
		wake_up_interruptible(&gpio->change_wq);

		if (gpio->async_queue)
			kill_fasync(&gpio->async_queue, SIGIO, POLL_IN);
	}

	return IRQ_HANDLED;
}

/* Handles open() done on /dev/gpioxx */
static int armadeus_gpio_dev_open(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	unsigned int major = MAJOR(inode->i_rdev);
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

	switch (minor)
	{
		/* Write all port pins in one time */
		case FULL_PORTA_MINOR:
		case FULL_PORTB_MINOR:
		case FULL_PORTC_MINOR:
		case FULL_PORTD_MINOR:
			gpio->nb_pins = 32;
			gpio->changed = 1;
			gpio->port    = FULL_MINOR_TO_PORT(minor);
			printk("Reserving full %s\n", port_name[gpio->port]);
			goto success;
		break;
	}

	/* Pin by pin access */
	gpio->nb_pins = 1;
	gpio->changed = 1;

	if( imx_gpio_request(minor, "gpio-dev") )
		goto err_request;

	gpio->port   = minor >> GPIO_PORT_SHIFT;
	gpio->number = minor & GPIO_PIN_MASK;
	gpio->pin_state = 0;

	if( getPortDir(gpio->port) & (1 << gpio->number) ) {
		imx_gpio_direction_output( minor, 0 );
	} else {
		imx_gpio_direction_input( minor );
		gpio->pin_state = imx_gpio_get_value( minor );
	}

	/* Request interrupt if pin was configured for */
	if (shadows_irq[gpio->port] & (1 << gpio->number) ) {
		irq = IRQ_GPIOA(minor); /* irq number are continuous */
		ret = request_irq( irq, armadeus_gpio_interrupt, 0, "gpio", gpio );
		if( ret )
			goto err_irq;
		/* set_irq_type( irq, IRQF_TRIGGER_FALLING ); TDBJUJU */
	}

success:
	printk("Opening /dev/gpio%d/%d file port:%d pin:%d\n", major, minor, gpio->port, gpio->number);
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

	if( gpio->initialized ) {
		if (shadows_irq[gpio->port] & (1 << gpio->number) ) {
			irq = IRQ_GPIOA(minor);
			free_irq( irq, gpio );
		}
		imx_gpio_free( minor );
	}
	kfree(gpio);
	printk("Closing access to /dev/gpio%d\n", minor);

	return 0;
}

/* Handling of IOCTL calls */
int armadeus_gpio_dev_ioctl( struct inode *inode, struct file *filp,
	unsigned int cmd, unsigned long arg )
{
	int err = 0; int ret = 0;
	int value=0;
	unsigned int minor;

	printk( DRIVER_NAME " ## IOCTL received: (0x%x) ##\n", cmd );

	/* Extract the type and number bitfields, and don't decode wrong cmds:
	   return ENOTTY (inappropriate ioctl) before access_ok() */
	if (_IOC_TYPE(cmd) != PP_IOCTL) return -ENOTTY;
	
	/* The direction is a bitmask, and VERIFY_WRITE catches R/W transfers.
	`Type' is user-oriented, while access_ok is kernel-oriented,
	so the concept of "read" and "write" is reversed */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));

	if (err) return -EFAULT;

	/* Obtain exclusive access */
	if (down_interruptible(&gpio_sema))
		return -ERESTARTSYS;
	/* Extract and test minor */
	minor = MINOR(inode->i_rdev);
	if (minor < FULL_PORTD_MINOR) {
		printk("Minor outside range: %d !\n", minor);
		return -EFAULT;
	}

	switch(cmd)
	{
		case GPIORDDIRECTION:
		value = getPortDir( minor );
		ret = __put_user(value, (unsigned int *)arg);
		break;
	
		case GPIOWRDIRECTION:
		ret = __get_user(value, (unsigned int *)arg);
	
		if (ret==0) {
			setPortDir( minor, value );
		}
		break;
	
		case GPIORDDATA:
		value = readFromPort( minor );
		ret = __put_user(value, (unsigned int *)arg);
		break;
	
		case GPIOWRDATA:
		ret = __get_user(value, (unsigned int *)arg);
		if (ret == 0) {
			writeOnPort( minor, value );
		}
		break;
	
		default:
		return -ENOTTY;
		break;
	}
	/* Release exclusive access */
	up(&gpio_sema);
	
	return ret;
}


/*
 * PROC file functions
 */

static int armadeus_gpio_proc_read(char *buffer, char **start, off_t offset,
	int buffer_length, int *eof, __attribute__ ((unused)) void *data)
{
	int len = 0; /* The number of bytes actually used */
	unsigned int port_status = 0x66;
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

	switch( settings->type )
	{
		case VALUE:
			port_status = readFromPort( port_ID );
		break;

		case DIRECTION:
			/* Get the status of the gpio direction registers TBDNICO */
			printk("direction\n");
			port_status = getPortDir(port_ID);
		break;

		case PULL_UP:
			port_status = PUEN(port_ID);
			printk("pull-up\n");
		break;

		case INTERRUPT:
			port_status = shadows_irq[port_ID];
			printk("interrupt\n");
		break;

		default:
			printk("%s: unknown setting\n", __FUNCTION__);
		break;
	}
	/* Put result to given chr buffer */
	len = toString(port_status, buffer, number_of_pins[port_ID]);
	printk("0x%08x\n", port_status);

	*eof = 1;
	up(&gpio_sema);
	
	/* Return the length */
	return len;
}

static char new_gpio_state[MAX_NUMBER_OF_PINS*2];

static int armadeus_gpio_proc_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, __attribute__ ((unused)) void* data)
{
	int len;
	unsigned int  gpio_state;
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
	if( down_interruptible(&gpio_sema) )
		return -ERESTARTSYS;

	/* Get datas to write from user space */
	if (copy_from_user(new_gpio_state, buf, len)) {
		up(&gpio_sema);
		return -EFAULT;
	}

	if( strlen(new_gpio_state) > 0 )
	{
		/* Convert it from String to Int */
		gpio_state = fromString( new_gpio_state, number_of_pins[port_ID] );

		switch( settings->type )
		{
			case VALUE:
				printk("value\n");
				writeOnPort( port_ID, gpio_state );
			break;

			case DIRECTION:
				printk("direction\n");
				setPortDir( port_ID, gpio_state );
			break;

			case PULL_UP:
				printk("pull-up\n");
			break;

			case INTERRUPT:
				shadows_irq[port_ID] = gpio_state;
				printk("interrupt\n");
			break;

			default:
				printk("%s: unknown setting\n", __FUNCTION__);
			break;
		}

		printk("/proc wrote 0x%x on %s %s register\n", gpio_state,
			port_name[port_ID], port_setting_name[settings->type]);
    }

	up(&gpio_sema);
	/* Makes as if we take all the data sent even if we can't handle more 
	than register size */
	return count;
}


/* /proc registering helpers */
struct proc_config_entry {
	struct proc_dir_entry *entry;
	char* name;
	int port;
	int type;
};

static int initialize_entry( struct proc_config_entry *config )
{
	int i;

	for (i = 0; i < 4; i++)
	{
		config[i].entry = create_proc_entry( config[i].name, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
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
	int ret;

	printk("Creating /proc entries: ");

	/* Create main directory */
	proc_mkdir(GPIO_PROC_DIRNAME, NULL);

	/* Create proc file to handle GPIO values */
	proc_config[PORT_A].name = GPIO_PROC_PORTA_FILENAME;
	proc_config[PORT_A].type = VALUE;
	proc_config[PORT_B].name = GPIO_PROC_PORTB_FILENAME;
	proc_config[PORT_B].type = VALUE;
	proc_config[PORT_C].name = GPIO_PROC_PORTC_FILENAME;
	proc_config[PORT_C].type = VALUE;
	proc_config[PORT_D].name = GPIO_PROC_PORTD_FILENAME;
	proc_config[PORT_D].type = VALUE;

	if ((ret = initialize_entry(proc_config)))
		return ret;
	init_map |= GPIO_PROC_FILE;

	/* Create proc file to handle GPIO direction settings */
	proc_config[PORT_A].name = GPIO_PROC_PORTADIR_FILENAME;
	proc_config[PORT_A].type = DIRECTION;
	proc_config[PORT_B].name = GPIO_PROC_PORTBDIR_FILENAME;
	proc_config[PORT_B].type = DIRECTION;
	proc_config[PORT_C].name = GPIO_PROC_PORTCDIR_FILENAME;
	proc_config[PORT_C].type = DIRECTION;
	proc_config[PORT_D].name = GPIO_PROC_PORTDDIR_FILENAME;
	proc_config[PORT_D].type = DIRECTION;

	if ((ret = initialize_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_PROC_FILE;

	/* Create proc file to handle GPIO interrupt settings */
	proc_config[PORT_A].name = GPIO_PROC_PORTA_IRQ_FILENAME;
	proc_config[PORT_A].type = INTERRUPT;
	proc_config[PORT_B].name = GPIO_PROC_PORTB_IRQ_FILENAME;
	proc_config[PORT_B].type = INTERRUPT;
	proc_config[PORT_C].name = GPIO_PROC_PORTC_IRQ_FILENAME;
	proc_config[PORT_C].type = INTERRUPT;
	proc_config[PORT_D].name = GPIO_PROC_PORTD_IRQ_FILENAME;
	proc_config[PORT_D].type = INTERRUPT;

	if ((ret = initialize_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_IRQ_PROC_FILE;

	/* Create proc file to handle GPIO pullup settings */
	proc_config[PORT_A].name = GPIO_PROC_PORTA_PULLUP_FILENAME;
	proc_config[PORT_A].type = PULL_UP;
	proc_config[PORT_B].name = GPIO_PROC_PORTB_PULLUP_FILENAME;
	proc_config[PORT_B].type = PULL_UP;
	proc_config[PORT_C].name = GPIO_PROC_PORTC_PULLUP_FILENAME;
	proc_config[PORT_C].type = PULL_UP;
	proc_config[PORT_D].name = GPIO_PROC_PORTD_PULLUP_FILENAME;
	proc_config[PORT_D].type = PULL_UP;

	if ((ret = initialize_entry(proc_config)))
		return ret;
	init_map |= SETTINGS_PULLUP_PROC_FILE;

	printk("OK!\n");
	return(0);
}

static void remove_proc_entries(void)
{
	/* Remove /proc entries */
	if (init_map & GPIO_PROC_FILE) {
		printk( DRIVER_NAME " removing " GPIO_PROC_PORTA_FILENAME " & Co\n" );
		remove_proc_entry( GPIO_PROC_PORTA_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTB_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTC_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTD_FILENAME, NULL);
	}
	if (init_map & SETTINGS_PROC_FILE) {
		printk( DRIVER_NAME " removing " GPIO_PROC_PORTADIR_FILENAME " & Co\n" );
		remove_proc_entry( GPIO_PROC_PORTADIR_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTBDIR_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTCDIR_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTDDIR_FILENAME, NULL);
	}
	if (init_map & SETTINGS_IRQ_PROC_FILE) {
		printk( DRIVER_NAME " removing " GPIO_PROC_PORTA_IRQ_FILENAME " & Co\n" );
		remove_proc_entry( GPIO_PROC_PORTA_IRQ_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTB_IRQ_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTC_IRQ_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTD_IRQ_FILENAME, NULL);
	}
	if (init_map & SETTINGS_PULLUP_PROC_FILE) {
		printk( DRIVER_NAME " removing " GPIO_PROC_PORTA_PULLUP_FILENAME " & Co\n" );
		remove_proc_entry( GPIO_PROC_PORTA_PULLUP_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTB_PULLUP_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTC_PULLUP_FILENAME, NULL);
		remove_proc_entry( GPIO_PROC_PORTD_PULLUP_FILENAME, NULL);
	}

	remove_proc_entry(GPIO_PROC_DIRNAME, NULL);
}
/* /dev functionnalities supported: */
static struct file_operations gpio_fops = {
	.owner   = THIS_MODULE,
	.llseek  = no_llseek,
	.write   = armadeus_gpio_dev_write,
	.read    = armadeus_gpio_dev_read,
	.open    = armadeus_gpio_dev_open,
	.release = armadeus_gpio_dev_release,
	.ioctl   = armadeus_gpio_dev_ioctl,
};

/* Module's initialization */
static int __init armadeus_gpio_init(void)
{
	static int result, i;
	
	printk("Initializing Armadeus GPIOs driver\n");
	printk("    PortA Parameters (%i): ", portA_init_nb);
	for (i = 0; i < NB_CONFIG_REGS; i++) { printk(" 0x%x", portA_init[i]); } printk("\n");
	printk("    PortB Parameters (%i): ", portB_init_nb);
	for (i = 0; i < NB_CONFIG_REGS; i++ ) { printk(" 0x%x", portB_init[i]); } printk("\n");
	printk("    PortC Parameters (%i): ", portC_init_nb);
	for (i = 0; i < NB_CONFIG_REGS; i++ ) { printk(" 0x%x", portC_init[i]); } printk("\n");
	printk("    PortD Parameters (%i): ", portD_init_nb);
	for (i = 0; i < NB_CONFIG_REGS; i++ ) { printk(" 0x%x", portD_init[i]); } printk("\n");
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

	printk( DRIVER_NAME " " DRIVER_VERSION " successfully loaded !\n");
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
void gpioWriteOnPort( unsigned int aPort, unsigned int aValue )
{
    if( aPort >= NB_PORTS ) { aPort = NB_PORTS - 1; printk(DRIVER_NAME "port unknown !\n"); return; }
    //
    writeOnPort( aPort, aValue );
}

unsigned int gpioReadFromPort( unsigned int aPort )
{
    if( aPort >= NB_PORTS ) { aPort = NB_PORTS - 1; printk(DRIVER_NAME "port unknown !\n"); return(0); }
    else
    return( readFromPort( aPort ) );
}

void gpioSetPortDir( unsigned int aPort, unsigned int aDirMask )
{
    setPortDir( aPort, aDirMask );
}

unsigned int gpioGetPortDir( unsigned int aPort )
{
    return( getPortDir( aPort ) );
}

EXPORT_SYMBOL( gpioWriteOnPort );
EXPORT_SYMBOL( gpioReadFromPort );
EXPORT_SYMBOL( gpioSetPortDir );
EXPORT_SYMBOL( gpioGetPortDir );


module_init(armadeus_gpio_init);
module_exit(armadeus_gpio_cleanup);
MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain");
MODULE_DESCRIPTION("Armadeus GPIOs driver");
MODULE_LICENSE("GPL");
