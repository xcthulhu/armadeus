/*
 * Armadeus Max1027 driver (fast version)
 *
 * Copyright (C) 2006-2008 Armadeus Project / ArmadeusPort Systems
 * Authors: Nicolas Colombain / Julien Boibessot
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

#include <asm/arch/imx-regs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h> /* request_irq */
#include <asm/gpio.h>        /* imx_gpio_... */
#include <linux/cdev.h>      /* struct cdev */
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/spi/spi.h>
#include <asm/semaphore.h>

#include <linux/delay.h>

#include <linux/irq.h> /* set_irq_type */

#define DRIVER_NAME    "max1027"
#define DRIVER_VERSION "0.1"

#define MAX1027_EOC_IRQ  IRQ_GPIOA(10) // CSI_D6 = PortA 10    IRQ_GPIOA(1) can be used for test

/* Internal registers prefixes */
#define MAX1027_REG_CONV    0x80
#define MAX1027_REG_SETUP   0x40
#define MAX1027_REG_AVG     0x20
#define MAX1027_REG_RESET   0x10
#define MAX1027_REG_UNI     0x00
#define MAX1027_REG_BIPO    0x00

/* register specifics */
#define MAX1027_CONV_CHSEL(x) 		((x&0x0f)<<3)
#define MAX1027_CONV_SCAN(x)  		((x&0x03)<<1)
#define MAX1027_CONV_TEMP  			0x01
/* All channels and temperature are scanned per conversion */
#define MAX1027_CONV_DEFAULT 		(MAX1027_REG_CONV | \
									MAX1027_CONV_CHSEL(7) | \
									MAX1027_CONV_SCAN(0) | MAX1027_CONV_TEMP)

#define MAX1027_SETUP_CLKSEL(x) 	((x&0x03)<<4)
#define MAX1027_SETUP_REFSEL(x) 	((x&0x03)<<2)
#define MAX1027_SETUP_DIFFSEL(x)  	(x&0x03)
#define MAX1027_SETUP_DIFFSEL_MASK 	 0x03
/* Internal clock and single ended configuration following setup cmd */
#define MAX1027_SETUP_DEFAULT 		(MAX1027_REG_SETUP | \
										MAX1027_SETUP_CLKSEL(2) | \
										MAX1027_SETUP_DIFFSEL(2))

#define MAX1027_AVG_AVGON(x)  		((x&0x01)<<4)
#define MAX1027_AVG_NAVG(x)   		((x&0x03)<<2)
#define MAX1027_AVG_NSCAN(x)  		(x&0x03)

#define MAX1027_RESET_ALL 			(0x0|MAX1027_REG_RESET)
#define MAX1027_RESET_FIFO 			(0x8|MAX1027_REG_RESET)

#define MAX1027_UNI_UCH(x) 			((x&0x0f)<<4)
#define MAX1027_BIPO_BCH(x) 		((x&0x0f)<<4)

#define NB_CHANNELS 				8
#define RESULT_ARRAY_SIZE 			(NB_CHANNELS+1)


// Global variables
struct max1027_operations *driver_ops;
static int max1027_major = 0; /* Dynamic major allocation */


struct max1027_results {
	u16 temp;             /* in 1/8 °C */
	u16 ain[NB_CHANNELS];
};

struct adc_channel {
	spinlock_t lock;

	int id;
	int enabled;
	int initialized;
	int running;
	int eoc; /* 1 if convertion ended */

	wait_queue_head_t change_wq;
	struct fasync_struct *async_queue;

	struct spi_dev *spi; /* clean ?? */
};

struct max1027 {
	struct class_device *cdev;
	struct mutex update_lock;
	struct max1027_results results;
	struct adc_channel* channels[NB_CHANNELS];
	int scan_time; /* in jiffies */
	unsigned long last_updated; /* in jiffies */
	/* Shadow registers to hold current configuration */
	u8 conv_reg;
	u8 setup_reg;
	u8 avg_reg;
	u8 reset_reg;
	u8 uni_reg;
	u8 bi_reg;

	struct tasklet_struct tasklet;
	int conversion_running;
};
#define GET_NB_CHANNELS(max1027) (max1027->conv_reg & MASK >> XX)

#define CS_CHANGE(val)			0  /* <-- ça sert à quoi ce truc ??? */

static struct spi_device *current_spi;

struct spi_transfer transfer[NB_CHANNELS+1];
struct spi_message message;
u8 buffer[32];


// must be used within mutex and outside of IRQ context !!!
static void max1027_send_cmd( struct spi_device *spi, u8 cmd )
{
	u8 buf = cmd;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);

	spi_write_then_read(spi, &buf, 1, NULL, 0);

	if( cmd & MAX1027_REG_CONV ) {
		p_max1027->conv_reg = cmd;
	}
	if( cmd & MAX1027_REG_SETUP ) {
		p_max1027->setup_reg = cmd;
	}
	if( cmd & MAX1027_REG_AVG ) {
		p_max1027->avg_reg = cmd;
	}
}

static void max1027_process_results(struct max1027 *max1027)
{
	struct adc_channel *channel;
	u8 msb, lsb;
	u16 value;
	int i=0, values_to_read=0, start=0;

	pr_debug("%s", __FUNCTION__);

	if( max1027->conv_reg & MAX1027_CONV_TEMP )
		values_to_read = NB_CHANNELS+1;
	else
		values_to_read = NB_CHANNELS;

#ifdef DEBUG
	for( i=0; i<values_to_read*2; i++ ) {
		printk("%02x ", buffer[i]);
	}
	printk("\n");
#endif

	start = 0;
	/* temp */
	if( max1027->conv_reg & MAX1027_CONV_TEMP ) {
		start = 1;
		msb = buffer[0] & 0x0f;
		lsb = buffer[1];
		value = ((msb << 8) | lsb);
		max1027->results.temp = value;
		printk("%d°C ", value >> 3); /* 1 unit = 1/8 °C */
	}

	for( i=start; i<values_to_read; i++ ) {
		msb = buffer[i*2] & 0x0f;
		lsb = buffer[(i*2)+1] >> 2;
		value = (msb << 8) | lsb;
		max1027->results.ain[i-start] = value;
		printk("0x%04x ", value);
	}
	printk("\n");


	/* Wake up waiting userspace apps */
	pr_debug("EOC for");
	for (i=0; i < NB_CHANNELS; i++ ) {
		channel = max1027->channels[i];
		if (channel != NULL) {
			if ( channel->running ) {
				wake_up_interruptible( &(channel->change_wq) );
				// if (channel->async_queue)
				//     kill_fasync(&channel->async_queue, SIGIO, POLL_IN);
				pr_debug(" %d", i);
				channel->running = 0;
			}
		}
	}
	pr_debug("\n");
	max1027->conversion_running = 0;
}

static void max1027_reads_async( struct spi_device *spi, int num_values )
{
	int ret, i;

	spi_message_init(&message);
	message.complete = (void (*)(void *)) max1027_process_results; // (void (*)(void *)) complete;
	message.context  = dev_get_drvdata(&current_spi->dev); /* cradingue ? */

	i = 0;
	while (i < num_values) {
		transfer[i].tx_buf = 0;
		transfer[i].rx_buf = buffer+(i*2);
		transfer[i].len = 2;
		transfer[i].cs_change = CS_CHANGE(i == num_values);
		spi_message_add_tail(&transfer[i++], &message);
	}

	ret = spi_async(spi, &message);
	if (ret)
		printk(KERN_ERR "%s: error %i in SPI request\n",
				__FUNCTION__, ret);
}


/*
 * Handles read() done on /dev/gpioxx
 */
static ssize_t max1027_dev_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	unsigned minor = MINOR(file->f_dentry->d_inode->i_rdev);
	u32 value=0;
	ssize_t ret = 0;
	struct adc_channel *channel = file->private_data;
	struct max1027 *max1027 = dev_get_drvdata(&current_spi->dev); /* cradingue */
	
	if (count == 0)
		return count;

	pr_debug("- %s %d byte(s) on minor %d -> channel %d\n", __FUNCTION__, count, minor, channel->id);

	/* Launch conversion */
	mutex_lock(&max1027->update_lock);
	if ( !max1027->conversion_running ) {
		max1027->conversion_running = 1;
		max1027_send_cmd( current_spi, MAX1027_REG_CONV | max1027->conv_reg );
	}
	mutex_unlock(&max1027->update_lock);

	spin_lock_irq(&channel->lock);
	channel->running = 1;

	/* Wait until conversion has been done (IRQ triggered) */
	while (channel->running) {
		spin_unlock_irq(&channel->lock);

		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(channel->change_wq, !channel->running))
			return -ERESTARTSYS;

		spin_lock_irq(&channel->lock);
	}
	channel->running = 0;

	/* Read conversion results */
	value = max1027->results.ain[channel->id]; //get_value_for_channel(channel->id);

	/* Copy result to given userspace buffer */
	count = min( count, (size_t)sizeof(u32) );
	if ( copy_to_user(buf, &value, count) ) {
		ret = -EFAULT;
		goto out;
	}
	ret = count;

out:
	spin_unlock_irq(&channel->lock);
	return ret;
}

/* Tasklet to get results after EOC IRQ */
/* Tasklets seems to be unable to call spi_xxx too :-( */
static void read_conversion_results( unsigned long data )
{
	struct spi_device *spi = (struct spi_device *)data;
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);

	/* Get conversion results from chip */
	if( max1027->conv_reg & MAX1027_CONV_TEMP )
		max1027_reads_async(spi, 9);
	else
		max1027_reads_async(spi, 8);
}

/* EOC (End Of Conversion) IRQ handler */
static irqreturn_t max1027_interrupt(int irq, void *dev_id)
{
	struct max1027 *max1027 = dev_id;

	/* schedule task for reading results from chip outside of IRQ */
	tasklet_schedule( &max1027->tasklet );

	return IRQ_HANDLED;
}


static int max1027_dev_open(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	int ret = 0;
	struct adc_channel *channel;
	struct max1027 *max1027 = dev_get_drvdata(&current_spi->dev); /* suite du cradingue */

	channel = kzalloc(sizeof(struct adc_channel), GFP_KERNEL);
	if (!channel)
		goto err_request;

	file->private_data = channel;
	spin_lock_init(&channel->lock);
	init_waitqueue_head(&channel->change_wq);

	channel->id = minor;

	/* Something to configure ?? */
	channel->enabled = 1;
	channel->running = 0;

	pr_debug("Opening /dev/max1027/AN%d\n", minor);
	channel->initialized = 1;
	max1027->channels[minor & 0x07] = channel;

	/* Activate EOC IRQ here ?!! */

	return 0;

err_request:
	/* what about spinlock & wait_queue ?? */
	return ret;
}

static int max1027_dev_release(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	struct adc_channel *channel = file->private_data;

	if( channel->initialized ) {
		/* something to do here ?? */
	}
	kfree(channel);
	pr_debug("Closing access to /dev/max1027/AN%d\n", minor);

	return 0;
}

static int max1027_dev_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
	int ret = 0;

	printk( DRIVER_NAME " ## IOCTL received: (0x%x) ##\n", cmd );

	return ret;
}

static struct file_operations max1027_fops = {
	.owner   = THIS_MODULE,
	.llseek  = no_llseek,
//	.write   = no_write,
	.read    = max1027_dev_read,
	.open    = max1027_dev_open,
	.release = max1027_dev_release,
	.ioctl   = max1027_dev_ioctl,
};


// buf value <256 -> max1027 config register
static ssize_t max1027_set_conversion(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	int val, reg;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	val = simple_strtol(buf, NULL, 10);
	printk("%s: 0x%02x\n", __FUNCTION__, val);

	mutex_lock(&p_max1027->update_lock);
	reg = MAX1027_REG_CONV | (val&0xff);
	max1027_send_cmd( spi, reg );
	p_max1027->conv_reg = (u8)reg;
	mutex_unlock(&p_max1027->update_lock);

	return count;
}

static ssize_t max1027_get_conversion(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	ret_size = sprintf(buf,"0x%2x", p_max1027->conv_reg);

	return ret_size;
}


// buf value <256 -> max1027 config register
// buf value >=256 -> max1027 Unipolar or Bipolar mode registers
static ssize_t max1027_set_setup(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	int val, reg;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	val = simple_strtol(buf, NULL, 10);
	printk("%s: 0x%02x", __FUNCTION__, val);

	mutex_lock(&p_max1027->update_lock);
	reg = MAX1027_REG_SETUP | (val&0xff);
	max1027_send_cmd( spi, reg );
	p_max1027->setup_reg = (u8)reg;
	// check whether we need to configure the uni or bipolar mode IOs or not
	if( ((val&0xff) & MAX1027_SETUP_DIFFSEL_MASK) > MAX1027_SETUP_DIFFSEL(1) ) {
		max1027_send_cmd( spi, val>>8 );
		printk("+ 0x%02x", val);
	}
	mutex_unlock(&p_max1027->update_lock);

	printk("\n");
	return count;
}

static ssize_t max1027_get_setup(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	ret_size = sprintf(buf,"0x%2x", p_max1027->setup_reg);

	return ret_size;
}

// buf value <256 -> max1027 averaging register
static ssize_t max1027_set_averaging(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	int val;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	val = simple_strtol(buf, NULL, 10);
	printk("%s: 0x%02x\n", __FUNCTION__, val);

	mutex_lock(&p_max1027->update_lock);
	max1027_send_cmd( spi, MAX1027_REG_AVG | (val&0xff) );
	mutex_unlock(&p_max1027->update_lock);

	return count;
}

static ssize_t max1027_get_averaging(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);
	
	ret_size = sprintf(buf,"0x%2x", p_max1027->avg_reg);

	return ret_size;
}

static DEVICE_ATTR(conversion, S_IWUSR | S_IRUGO, max1027_get_conversion, max1027_set_conversion);
static DEVICE_ATTR(setup,      S_IWUSR | S_IRUGO, max1027_get_setup, max1027_set_setup);
static DEVICE_ATTR(averaging,  S_IWUSR | S_IRUGO, max1027_get_averaging, max1027_set_averaging);
/* Unipolar / Bipolar are set with Setup register specials modes !!!
static DEVICE_ATTR(unipolar,   S_IWUSR, NULL, max1027_unipolar);
static DEVICE_ATTR(bipolar,    S_IWUSR, NULL, max1027_bipolar);*/

#define SYSFS_ERROR_STRING "Unable to create sysfs attribute for max1027"

int static max1027_create_sys_entries(struct spi_device *spi)
{
	int status = 0;

	if ((status = device_create_file(&spi->dev, &dev_attr_conversion))) {
		printk(KERN_WARNING SYSFS_ERROR_STRING " Conversion register\n");
		goto end;
	}

	if ((status = device_create_file(&spi->dev, &dev_attr_setup))) {
		printk(KERN_WARNING SYSFS_ERROR_STRING " Setup register\n");
		goto end;
	}

	if ((status = device_create_file(&spi->dev, &dev_attr_averaging))) {
		printk(KERN_WARNING SYSFS_ERROR_STRING " Averaging register\n");
		goto end;
	}

end:
	return status;
}

static int __devinit max1027_probe(struct spi_device *spi)
{
	struct max1027 *max1027;
	static int result;
	int i;
	
	pr_debug("%s", __FUNCTION__);

	max1027 = kzalloc(sizeof(struct max1027), GFP_KERNEL);
	if (!max1027)
		return -ENOMEM;
	mutex_init(&max1027->update_lock);
	for (i=0; i < NB_CHANNELS; i++ ) {
		max1027->channels[i] = NULL;
	}

	/* Register the driver as character device by getting a major number */
	result = register_chrdev(max1027_major, DRIVER_NAME, &max1027_fops);
	if (result < 0) 
	{
		printk(KERN_WARNING DRIVER_NAME ": can't get major %d\n", max1027_major);
		return result;
	}
	if( max1027_major == 0 ) max1027_major = result; /* dynamic Major allocation */

	/* Request interrupt for EOC */
	result = request_irq( MAX1027_EOC_IRQ , max1027_interrupt, 0, DRIVER_NAME, max1027 );
	if( result )
		goto err_irq;
	imx_gpio_mode( GPIO_PORTA | 10 | GPIO_IN | GPIO_GIUS );
	set_irq_type( MAX1027_EOC_IRQ, IRQF_TRIGGER_FALLING );

	/* Create /sys entries */
	/* sysfs hook */
	dev_set_drvdata(&spi->dev, max1027);
	current_spi = spi; /* <<-- cradingue !! trouver comment faire autrement !! */

	result = max1027_create_sys_entries(spi);
	if (result)
		goto err_sys;

	tasklet_init( &max1027->tasklet, read_conversion_results, (unsigned long)spi);
	max1027->conversion_running = 0;

	/* setup spi_device */
	spi->bits_per_word = 8;
	spi_setup(spi);
	/* configure the MAX */
	max1027_send_cmd( spi, MAX1027_RESET_ALL );
	max1027->conv_reg = (u8)MAX1027_CONV_DEFAULT;
	max1027_send_cmd( spi, MAX1027_SETUP_DEFAULT );

	printk( DRIVER_NAME " v" DRIVER_VERSION " successfully probed !\n");
	return(0);

err_sys:
	free_irq(MAX1027_EOC_IRQ, max1027);
err_irq:
	unregister_chrdev(max1027_major, DRIVER_NAME);

	return result;
}

static int __devexit max1027_remove(struct spi_device *spi)
{
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);
	int i;

	pr_debug("%s\n", __FUNCTION__);

	device_remove_file(&spi->dev, &dev_attr_conversion);
	device_remove_file(&spi->dev, &dev_attr_setup);
	device_remove_file(&spi->dev, &dev_attr_averaging);

	tasklet_kill( &max1027->tasklet );

	/* De-register from /dev interface */
	unregister_chrdev(max1027_major, DRIVER_NAME);

	free_irq(MAX1027_EOC_IRQ, max1027);

	dev_set_drvdata(&spi->dev, NULL);

	for (i=0; i < NB_CHANNELS; i++) {
		kfree( max1027->channels[i] );
	}
	kfree(max1027);

	return 0;
}


static struct spi_driver max1027_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe	= max1027_probe,
	.remove	= __devexit_p(max1027_remove),
};

static int __init max1027_init(void)
{
	return spi_register_driver(&max1027_driver);
}

static void __exit max1027_exit(void)
{
	spi_unregister_driver(&max1027_driver);
}

module_init(max1027_init);
module_exit(max1027_exit);


MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain - Armadeus Systems");
MODULE_DESCRIPTION("Max1027 driver");
MODULE_LICENSE("GPL");
