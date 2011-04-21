/*
 * MAX1027 (ADC) driver for kernel >= 2.6.23
 *
 * Copyright (C) 2006-2008 Armadeus Project / Armadeus Systems
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

/*#define DEBUG 1*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h> /* request_irq */
#include <linux/version.h>
#include <linux/spi/max1027.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/irq.h> /* set_irq_type */
#if defined(CONFIG_HWMON) || defined(CONFIG_HWMON_MODULE)
#include <linux/hwmon.h>
#endif

#include <mach/gpio.h>
#include <asm/uaccess.h>     /* copy_to_user */

#define DRIVER_NAME    "max1027"
#define DRIVER_VERSION "0.71"


/* Internal registers prefixes */
#define MAX1027_REG_CONV    0x80
#define MAX1027_REG_SETUP   0x40
#define MAX1027_REG_AVG     0x20
#define MAX1027_REG_RESET   0x10
#define MAX1027_REG_UNI     0x00
#define MAX1027_REG_BIPO    0x00

/* register specifics */
#define MAX1027_REG_CONV_SCAN1		(1 << 2)
#define MAX1027_REG_SETUP_CKSEL1	(1 << 5)
#define GET_SELECTED_CHANNEL(conv)	( (conv & 0x78) >> 3 )
#define MAX1027_SETUP_DIFFSEL_MASK 	0x03
#define GET_NB_SCAN(avg)		( 4 * ((avg & 0x03)+1) )
#define MAX1027_RESET_ALL		(0x0|MAX1027_REG_RESET)
#define MAX1027_RESET_FIFO		(0x8|MAX1027_REG_RESET)
#define MAX1027_UNI_UCH(x)		((x&0x0f)<<4)
#define MAX1027_BIPO_BCH(x)		((x&0x0f)<<4)

#define NB_CHANNELS			8
#define MAX_RESULTS_PER_CHANNEL		16   /* NSCAN = 11 */
#define MAX_NB_RESULTS			(MAX_RESULTS_PER_CHANNEL+1) /* + temp */

#define FIFO_SIZE 128


/* Global variables */
struct max1027_operations *driver_ops;
static int max1027_major = 0; /* Dynamic major allocation */

struct adc_channel {
	int id;  /* channel  ID */
#define DATA_AVAILABLE 0
	unsigned long status;

	/* channel FIFO */
	volatile unsigned int head, tail;
	int nb_data_required;
	volatile u16 buffer[FIFO_SIZE];

	/* channel wait queue */
	wait_queue_head_t change_wq;
	struct spi_dev *spi; /* clean ?? */
};

struct max1027 {
	struct device *cdev;
	struct mutex update_lock;

#define CONVERSION_RUNNING 0
	unsigned long status;

	u16 ain[NB_CHANNELS]; /* latest channels value */
	struct adc_channel* channels[NB_CHANNELS];
	int temperature; /* current one in millidegree Celcius */
	/* Shadow registers to hold current configuration */
	u8 conv_reg;
	u8 setup_reg;
	u8 avg_reg;
	int cnvst; /* conversion start pin */

	struct tasklet_struct tasklet;

	wait_queue_head_t conv_wq;	/* stores waiters of end of conv */
	int missed_eoc;			/* stores all missed EOC interrupts */
};

#define CS_CHANGE(val)			0  /* <-- what is it for ??? */

static struct spi_device *current_spi;

struct spi_transfer transfer[MAX_NB_RESULTS];
struct spi_message message;
u8 buffer[MAX_NB_RESULTS*2]; /* each result is sent with 16 bits */


static inline void trig_cnvst(struct max1027* max1027)
{	
	gpio_set_value(max1027->cnvst, 0); /* Platform specific no ? */
	udelay(1);
	gpio_set_value(max1027->cnvst, 1);
}

static void max1027_start_conv(struct max1027*, struct adc_channel*);

static void fifo_flush(struct adc_channel* channel)
{
	channel->head = 0;
	channel->tail = 0; 
	channel->nb_data_required = 0;
}

static u16 fifo_inuse(struct adc_channel* channel)
{
	return channel->head - channel->tail; 
}

static void fifo_put(u16 c, struct adc_channel* channel)
{
	if (fifo_inuse(channel) != FIFO_SIZE) {
		channel->buffer[channel->head++%FIFO_SIZE] = c;
		if (channel->nb_data_required)
			channel->nb_data_required--;
	} else {
		pr_debug("put fifo full %d\n", channel->id);
		printk("put fifo full %d\n", channel->id);
	}

	if (channel->nb_data_required <= 0) {
		set_bit(DATA_AVAILABLE, &channel->status);
		wake_up_interruptible(&(channel->change_wq));
	}
}

static u16 fifo_get(u16 * c, u16 count, struct adc_channel* channel)
{
	int j, i=0;

	channel->nb_data_required = 0;
	j = fifo_inuse(channel);
	for (i = 0; i < min(j, (int)count); i++) {
		c[i] = channel->buffer[channel->tail++%FIFO_SIZE];
	}
	channel->nb_data_required = min(count-i, FIFO_SIZE);

	return i;
}

static void inline max1027_wait_end_of_conv(struct max1027 *max1027)
{
	int delay;

	delay = wait_event_timeout(max1027->conv_wq, max1027->status==0, msecs_to_jiffies(20));
	if (delay == 0) {
		printk(KERN_WARNING "error: missed conversion %lu ", max1027->status);
		if (test_bit(CONVERSION_RUNNING, &max1027->status)) {
			max1027->missed_eoc++;
			clear_bit(CONVERSION_RUNNING, &max1027->status);
		}
	}

	/*printk("delay %lu\n", delay);*/
}

/* Must be used within mutex and outside of IRQ context !!! */
static void max1027_send_cmd(struct spi_device *spi, u8 cmd)
{
	u8 buf = cmd;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);

	/* !! order is important here !! ;-) */
	if (cmd & MAX1027_REG_CONV) {
		p_max1027->conv_reg = cmd;
	}
	else if (cmd & MAX1027_REG_SETUP) {
		p_max1027->setup_reg = cmd;
	}
	else if (cmd & MAX1027_REG_AVG) {
		p_max1027->avg_reg = cmd;
	}

	spi_write_then_read(spi, &buf, 1, NULL, 0);
}

static void max1027_start_conv(struct max1027 *max1027, struct adc_channel *channel)
{
	if (test_and_set_bit(CONVERSION_RUNNING, &max1027->status)) {
		printk("conv already running\n");
		return;
	}

	/* No conversion running: launch one */

	if (channel)
		clear_bit(DATA_AVAILABLE, &channel->status);
	/* Use CNVST triggered conv. */
	trig_cnvst(max1027);
} 

/* Called when SPI got results from MAX1027 */
static void max1027_process_results(struct max1027 *max1027)
{
	u8 msb, lsb;
	u16 value;
	int i=0, values_to_read=0, start=0, nb_data_required=0;
	unsigned int scan_mode, selected_channel;

	pr_debug("%s: ", __FUNCTION__);

	selected_channel = GET_SELECTED_CHANNEL(max1027->conv_reg);
	if (selected_channel >= NB_CHANNELS)
		selected_channel = NB_CHANNELS - 1;
	scan_mode = GET_SCAN_MODE(max1027->conv_reg);
	if (scan_mode == SCAN_MODE_00) {
		values_to_read = selected_channel + 1;
	} else if (scan_mode == SCAN_MODE_01) {
		values_to_read = NB_CHANNELS - selected_channel + 1;
	} else {
		values_to_read = 1;
	}

	if (max1027->conv_reg & MAX1027_CONV_TEMP)
		values_to_read += 1;
#ifdef DEBUG
	for (i=0; i<values_to_read*2; i++) {
		printk("%02x ", buffer[i]);
	}
	printk("\n");
#endif

	start = 0;
	/* temp */
	if (max1027->conv_reg & MAX1027_CONV_TEMP) {
		msb = buffer[0] & 0x0f;
		lsb = buffer[1];
		value = ((msb << 8) | lsb);
		max1027->temperature = (value * 1000) >> 3; /* 1 unit = 1/8 °C + save it in millidegree */
		pr_debug("%d m°C ", max1027->temperature);
		start = 1;
	}
	nb_data_required = 0;
	for (i=start; i<values_to_read; i++) {
		msb = buffer[i*2] & 0x0f;
		lsb = buffer[(i*2)+1];
		value = ((msb << 8) | lsb) >> 2;
		if (scan_mode == SCAN_MODE_00) {
			int id = i-start;
			if (max1027->channels[id] != NULL) {
				fifo_put(value, max1027->channels[id]);
				nb_data_required = max(nb_data_required, max1027->channels[id]->nb_data_required);
			}
			max1027->ain[i-start] = value;
		} else {
			if (max1027->channels[selected_channel] != NULL) {
				fifo_put(value, max1027->channels[selected_channel]);
				nb_data_required = max(nb_data_required, max1027->channels[selected_channel]->nb_data_required);
			}
			max1027->ain[selected_channel++] = value;
		}
		pr_debug("0x%04x ", value);
	}
	pr_debug("\n");

	if (nb_data_required) {
		max1027_start_conv(max1027, max1027->channels[selected_channel]);
	} else {
		clear_bit(CONVERSION_RUNNING, &max1027->status);
	}

	/* Somebody's waiting for the "global" End Of Conversion ? */
	wake_up(&(max1027->conv_wq));
}

static void max1027_reads_async(struct spi_device *spi, int num_values)
{
	int ret, i;

	spi_message_init(&message);
	message.complete = (void (*)(void *)) max1027_process_results;
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
 * Handles read() done on /dev/...
 */
static ssize_t max1027_dev_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	unsigned minor = MINOR(file->f_dentry->d_inode->i_rdev);
	u16 value[FIFO_SIZE];
	ssize_t ret = 0; 
	struct adc_channel *channel = file->private_data;
	struct max1027 *max1027 = dev_get_drvdata(&current_spi->dev); /* cradingue */
	
	if (count == 0)
		return count; 
	pr_debug("- %s %d byte(s) on minor %d -> channel %d\n", __FUNCTION__, count, minor, channel->id);

	if (fifo_inuse(channel) == 0) {
		max1027_start_conv(max1027, channel);

		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible(channel->change_wq, channel->status))
			return -ERESTARTSYS;
	}
	//printk("count dev %d\n", count);
	count = fifo_get(value, count>>1, channel)<<1;
	//printk("count %d\n", count);

	/* Copy result to given userspace buffer */
	//count = min( count, (size_t)sizeof(u16) );
	if (copy_to_user(buf, value, count)) {
		ret = -EFAULT;
		goto out;
	}
	ret = count;

out:
	return ret;
}

/* Tasklet to get results after EOC interrupt */
static void read_conversion_results(unsigned long data)
{
	struct spi_device *spi = (struct spi_device *)data;
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);
	int size = 0;

	/* Calculate parameters to get conversion results from chip
	   (depends on conversion mode) */
	switch (GET_SCAN_MODE(max1027->conv_reg))
	{
		case SCAN_MODE_00:
			size = GET_SELECTED_CHANNEL(max1027->conv_reg) + 1;
		break;

		case SCAN_MODE_01:
			size = NB_CHANNELS - GET_SELECTED_CHANNEL(max1027->conv_reg);
		break;

		case SCAN_MODE_10:
			size = GET_NB_SCAN(max1027->avg_reg);
		break;

		case SCAN_MODE_11:
			size = 1;
		break;

		default:
			printk(KERN_ERR "%s: max1027 scan mode not supported\n",
				__FUNCTION__);
		break;
	}

	/* Get the results */
	if (max1027->conv_reg & MAX1027_CONV_TEMP) {
		max1027_reads_async(spi, size+1);
	} else {
		max1027_reads_async(spi, size);
	}
}

/* EOC (End Of Conversion) IRQ handler */
static irqreturn_t max1027_interrupt(int irq, void *dev_id)
{
	struct max1027 *max1027 = dev_id;

	/* schedules task for reading conversion results outside of IRQ */
	tasklet_hi_schedule(&max1027->tasklet);

	return IRQ_HANDLED;
}


static void max1027_flush_all_channels(struct max1027 *p_max1027)
{
	int i;

	for (i=0; i < NB_CHANNELS; i++) {
		if (p_max1027->channels[i] != NULL)
			fifo_flush(p_max1027->channels[i]);
	}
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
	//spin_lock_init(&channel->lock);
	init_waitqueue_head(&channel->change_wq);

	channel->id = minor;
	channel->status = 0;

	pr_debug("Opening /dev/max1027/AN%d\n", minor);
	fifo_flush(channel);
	max1027->channels[minor & 0x07] = channel;

	return 0;

err_request:
	/* what about spinlock & wait_queue ?? */
	return ret;
}

static int max1027_dev_release(struct inode *inode, struct file *file)
{
	unsigned minor = MINOR(inode->i_rdev);
	struct adc_channel *channel = file->private_data;
	struct max1027 *max1027 = dev_get_drvdata(&current_spi->dev); /* suite du cradingue */

	mutex_lock(&max1027->update_lock);
	max1027->channels[channel->id] = NULL;
	kfree(channel);
	mutex_unlock(&max1027->update_lock);

	pr_debug("Closing access to /dev/max1027/AN%d\n", minor);
	return 0;
}

static struct file_operations max1027_fops = {
	.owner   = THIS_MODULE,
	.llseek  = no_llseek,
//	.write   = no_write,
	.read    = max1027_dev_read,
	.open    = max1027_dev_open,
	.release = max1027_dev_release,
};

/* sysfs hook functions */

/* Given value will be put in MAX1027 conversion register.
   Writing to this register will start an acquisition/conversion if
   clock mode 1x is configured in setup register.
*/
static ssize_t max1027_set_conversion(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);
	int val;

	/* Only 1 conversion can be launched that way at a given time */
	mutex_lock(&max1027->update_lock);

	val = (simple_strtol(buf, NULL, 10)) & 0xff;
	pr_debug("\n%s: 0x%02x\n", __FUNCTION__, val);
	max1027_flush_all_channels(max1027);
	/* Warn if a conversion is already launched and selected mode will
	   trigger a new one */
        if (test_and_set_bit(CONVERSION_RUNNING, &max1027->status)) {
		/* Should not occur !! */
		printk("%s: conv already running!\n", __func__);
	}
	/* Send value to chip */
	max1027_send_cmd(spi, MAX1027_REG_CONV | val);
	if (!(max1027->setup_reg & MAX1027_REG_SETUP_CKSEL1)) {
		if (max1027->cnvst == 0){
			printk(KERN_WARNING DRIVER_NAME "CNVST pin not defined!\n");
			return 0;
		}
		mdelay(1); 
		/* Use CNVST triggered conv. */
		trig_cnvst(max1027);
	}

	/* Wait until current convertion is finished if corresponding clock
	   mode is selected */
	max1027_wait_end_of_conv(max1027);
	pr_debug("%s end\n", __FUNCTION__);

	mutex_unlock(&max1027->update_lock);

	return count;
}

static ssize_t max1027_get_conversion(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);

	ret_size = sprintf(buf,"0x%02x", max1027->conv_reg);

	return ret_size;
}

/* buf value <256 -> accesss to "config" register
   buf value >=256 -> "Unipolar" or "Bipolar" mode registers */
static ssize_t max1027_set_setup(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);
	int val;

	val = simple_strtol(buf, NULL, 10);
	pr_debug("%s: 0x%02x", __FUNCTION__, val);

	mutex_lock(&max1027->update_lock);
	max1027_send_cmd(spi, MAX1027_REG_SETUP | (val&0xff));
	/* check whether we need to configure the uni or bipolar mode IOs or not */
	if (((val&0xff) & MAX1027_SETUP_DIFFSEL_MASK) > MAX1027_SETUP_DIFFSEL(1)) {
		max1027_send_cmd(spi, val>>8);
		pr_debug("+ 0x%02x", val);
	}
	max1027_flush_all_channels(max1027);
	mutex_unlock(&max1027->update_lock);

	pr_debug("\n");

	return count;
}

static ssize_t max1027_get_setup(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);

	ret_size = sprintf(buf,"0x%02x", p_max1027->setup_reg);

	return ret_size;
}

static ssize_t max1027_set_averaging(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct spi_device *spi = to_spi_device(dev);
	int val;
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);

	val = (simple_strtol(buf, NULL, 10)) & 0xff;
	pr_debug("%s: 0x%02x\n", __FUNCTION__, val);

	mutex_lock(&p_max1027->update_lock);
	max1027_send_cmd(spi, MAX1027_REG_AVG | val);
	max1027_flush_all_channels(p_max1027);
	mutex_unlock(&p_max1027->update_lock);

	return count;
}

static ssize_t max1027_get_averaging(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *p_max1027 = dev_get_drvdata(&spi->dev);

	ret_size = sprintf(buf, "0x%02x", p_max1027->avg_reg);

	return ret_size;
}

static DEVICE_ATTR(conversion, S_IWUSR | S_IRUGO, max1027_get_conversion, max1027_set_conversion);
static DEVICE_ATTR(setup,      S_IWUSR | S_IRUGO, max1027_get_setup, max1027_set_setup);
static DEVICE_ATTR(averaging,  S_IWUSR | S_IRUGO, max1027_get_averaging, max1027_set_averaging);
/* Unipolar / Bipolar are set with Setup register specials modes !!!
static DEVICE_ATTR(unipolar,   S_IWUSR, NULL, max1027_unipolar);
static DEVICE_ATTR(bipolar,    S_IWUSR, NULL, max1027_bipolar);*/

/* For debug: */
static ssize_t max1027_get_statistics(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t ret_size = 0;
        struct spi_device *spi = to_spi_device(dev);
        struct max1027 *max1027 = dev_get_drvdata(&spi->dev);

	ret_size = sprintf(buf, "%d", max1027->missed_eoc);

	return ret_size;
}

static ssize_t max1027_set_statistics(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
        struct spi_device *spi = to_spi_device(dev);
        struct max1027 *max1027 = dev_get_drvdata(&spi->dev);

	/* Whatever was given, clear statistics */
	max1027->missed_eoc = 0;

        return count;
}

static DEVICE_ATTR(statistics, S_IWUSR | S_IRUGO, max1027_get_statistics, max1027_set_statistics);

/* For channel values: */
#define show_in(offset) \
static ssize_t show_in##offset##_input(struct device *dev, \
					struct device_attribute *attr, char *buf) \
{ \
	int result; \
	ssize_t size; \
	struct spi_device *spi = to_spi_device(dev); \
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev); \
\
	result = max1027->ain[offset]; \
	size = sprintf(buf, "%d\n", (result*2500)>>10); /* millivolt with 2.5V ref */ \
\
	return size; \
} 

show_in(0);
show_in(1);
show_in(2);
show_in(3);
show_in(4);
show_in(5);
show_in(6);
show_in(7);

static DEVICE_ATTR(in0_input, S_IRUGO, show_in0_input, NULL);
static DEVICE_ATTR(in1_input, S_IRUGO, show_in1_input, NULL);
static DEVICE_ATTR(in2_input, S_IRUGO, show_in2_input, NULL);
static DEVICE_ATTR(in3_input, S_IRUGO, show_in3_input, NULL);
static DEVICE_ATTR(in4_input, S_IRUGO, show_in4_input, NULL);
static DEVICE_ATTR(in5_input, S_IRUGO, show_in5_input, NULL);
static DEVICE_ATTR(in6_input, S_IRUGO, show_in6_input, NULL);
static DEVICE_ATTR(in7_input, S_IRUGO, show_in7_input, NULL);

/* On chip temperature: */
static ssize_t max1027_get_temperature(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int result;
	ssize_t size;
	struct spi_device *spi = to_spi_device(dev);
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);

	result = max1027->temperature;
	size = sprintf(buf, "%d\n", result);

	return size;
}

static DEVICE_ATTR(temp1_input, S_IRUGO, max1027_get_temperature, NULL);


#define SYSFS_ERROR_STRING "Unable to create sysfs attribute for max1027"

#define CREATE_SYSFS_FOR_AIN(id) \
	if ((status = device_create_file(&spi->dev, &dev_attr_in##id##_input))) { \
		printk(KERN_WARNING SYSFS_ERROR_STRING "in##id## \n"); \
		goto end; \
	}

static int max1027_create_sys_entries(struct spi_device *spi)
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

        if ((status = device_create_file(&spi->dev, &dev_attr_statistics))) {
                printk(KERN_WARNING SYSFS_ERROR_STRING " missed EOC accessor\n");
                goto end;
        }

	if ((status = device_create_file(&spi->dev, &dev_attr_temp1_input))){ 
		printk(KERN_WARNING SYSFS_ERROR_STRING " temp1\n");
		goto end;
	}

	CREATE_SYSFS_FOR_AIN(0);
	CREATE_SYSFS_FOR_AIN(1);
	CREATE_SYSFS_FOR_AIN(2);
	CREATE_SYSFS_FOR_AIN(3);
	CREATE_SYSFS_FOR_AIN(4);
	CREATE_SYSFS_FOR_AIN(5);
	CREATE_SYSFS_FOR_AIN(6);
	CREATE_SYSFS_FOR_AIN(7);

end:
	return status;
}

static void max1027_remove_sys_entries(struct spi_device *spi)
{
	device_remove_file(&spi->dev, &dev_attr_conversion);
	device_remove_file(&spi->dev, &dev_attr_setup);
	device_remove_file(&spi->dev, &dev_attr_averaging);
	device_remove_file(&spi->dev, &dev_attr_statistics);
	device_remove_file(&spi->dev, &dev_attr_temp1_input);
	device_remove_file(&spi->dev, &dev_attr_in0_input);
	device_remove_file(&spi->dev, &dev_attr_in1_input);
	device_remove_file(&spi->dev, &dev_attr_in2_input);
	device_remove_file(&spi->dev, &dev_attr_in3_input);
	device_remove_file(&spi->dev, &dev_attr_in4_input);
	device_remove_file(&spi->dev, &dev_attr_in5_input);
	device_remove_file(&spi->dev, &dev_attr_in6_input);
	device_remove_file(&spi->dev, &dev_attr_in7_input);
}

static int __devinit max1027_probe(struct spi_device *spi)
{
	struct max1027 *max1027;
	struct max1027_config *platform_info; 
	int i, result;

	pr_debug("%s\n", __FUNCTION__);

	platform_info = (struct max1027_config *)(spi->dev.platform_data);
	if (!platform_info) {
		dev_err(&spi->dev, "can't get the platform data for MAX1027\n");
		return -EINVAL;
	}

	max1027 = kzalloc(sizeof(struct max1027), GFP_KERNEL);
	if (!max1027)
		return -ENOMEM;
	mutex_init(&max1027->update_lock);
	for (i=0; i < NB_CHANNELS; i++) {
		max1027->channels[i] = NULL;
	}

	/* Register the driver as character device by getting a major number */
	result = register_chrdev(max1027_major, DRIVER_NAME, &max1027_fops);
	if (result < 0) {
		printk(KERN_WARNING DRIVER_NAME ": can't get major %d\n", max1027_major);
		return result;
	}
	if (max1027_major == 0)
		max1027_major = result; /* dynamic Major allocation */

	/* Setup any GPIO active */
	result = platform_info->init(spi);
	if (result) {
		printk(KERN_WARNING DRIVER_NAME ": can't reserve gpios\n");
		goto err_irq;
	}
	max1027->cnvst = platform_info->cnvst_pin;
	if (max1027->cnvst > 0)
		gpio_set_value(max1027->cnvst, 1);

	/* Create /sys entries */
	/* sysfs hook */
	dev_set_drvdata(&spi->dev, max1027);
	current_spi = spi;

#if defined(CONFIG_HWMON) || defined(CONFIG_HWMON_MODULE)
	/* register to hwmon */
	max1027->cdev = hwmon_device_register(&spi->dev);
	if (IS_ERR(max1027->cdev)) {
		result = PTR_ERR(max1027->cdev);
		goto err_hwmon;
	}
#endif

	result = max1027_create_sys_entries(spi);
	if (result)
		goto err_sys;

	tasklet_init(&max1027->tasklet, read_conversion_results, (unsigned long)spi);
	max1027->status = 0;
	init_waitqueue_head(&max1027->conv_wq);

	/* setup spi_device */
	spi->bits_per_word = 8;
	spi_setup(spi);

	/* configure the MAX */
	max1027_send_cmd(spi, MAX1027_RESET_ALL);
	max1027_send_cmd(spi, platform_info->setup | MAX1027_REG_SETUP);
	max1027_send_cmd(spi, platform_info->avg | MAX1027_REG_AVG);
	max1027->conv_reg = (u8)(platform_info->conv | MAX1027_REG_CONV);

	set_irq_type(spi->irq, IRQF_TRIGGER_FALLING); /* machine specific... */
	/* Request interrupt for EOC */ /* Should be put before GPIO init no ? */
	result = request_irq(spi->irq, max1027_interrupt, 0, DRIVER_NAME, max1027);
	if (result)
		goto err_irq;

	printk(DRIVER_NAME " v" DRIVER_VERSION " successfully probed !\n");

	return 0;

err_sys:
	free_irq(spi->irq, max1027);
#if defined(CONFIG_HWMON) || defined(CONFIG_HWMON_MODULE)
err_hwmon:
	hwmon_device_unregister(max1027->cdev);
#endif
err_irq:
	unregister_chrdev(max1027_major, DRIVER_NAME);

	return result;
}

static int __devexit max1027_remove(struct spi_device *spi)
{
	int i;
	struct max1027 *max1027 = dev_get_drvdata(&spi->dev);
	struct max1027_config *platform_info =
		(struct max1027_config *)(spi->dev.platform_data);

	pr_debug("%s\n", __FUNCTION__);

	max1027_remove_sys_entries(spi);
#if defined(CONFIG_HWMON) || defined(CONFIG_HWMON_MODULE)
	hwmon_device_unregister(max1027->cdev);
#endif

	tasklet_kill(&max1027->tasklet);

	/* De-register from /dev interface */
	unregister_chrdev(max1027_major, DRIVER_NAME);

	free_irq(spi->irq, max1027);
	/* Free GPIOs*/
	platform_info->exit(spi);

	dev_set_drvdata(&spi->dev, NULL);

	for (i=0; i < NB_CHANNELS; i++) {
		kfree(max1027->channels[i]);
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
