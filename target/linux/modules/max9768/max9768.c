/*
 * MAX9768 audio amplifier driver
 *
 * Copyright (C) 2009 Armadeus Project / Armadeus Systems
 * Author: Nicolas Colombain
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
#include <linux/param.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/sysfs.h>
#include <mach/gpio.h>
#include <linux/err.h>

#include "max9768.h"

#define DRIVER_VERSION		"0.2"
#define DRIVER_NAME		"max9768"
#define MAX9768_VOLUME_MASK	0x3F
#define MAX9768_VOLUME_MAX	63
#define MAX9768_VOLUME_MIN	0

struct max9768 {
	struct i2c_client *client;
	int mute_pin;
	int shdn_pin;
	int current_volume;
	int shdn_state;
	int mute_state;
};

static void max9768_shutdown(struct max9768 *max9768, int shutdown)
{
	if (max9768->shdn_pin) {
		if (shutdown)
			gpio_set_value(max9768->shdn_pin, 0);
		else
			gpio_set_value(max9768->shdn_pin, 1);
	}
	max9768->shdn_state = shutdown;
}

static void max9768_mute(struct max9768 *max9768, int mute)
{
	if (max9768->mute_pin) {
		gpio_set_value(max9768->mute_pin, mute);
	}
	max9768->mute_state = mute;
}

static int max9768_send(struct i2c_client *client, int value)
{
	char msg = value & 0xff;

	return i2c_master_send(client, &msg, 1);
}


/* sysfs hook functions */

static ssize_t max9768_get_volume(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);

	return sprintf(buf, "%d", max9768->current_volume);
}

static ssize_t max9768_set_volume(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);
	char msg;
	int err;
	
	max9768->current_volume = simple_strtol(buf, NULL, 10);
	if (max9768->current_volume > MAX9768_VOLUME_MAX)
		max9768->current_volume = MAX9768_VOLUME_MAX;
	else if (max9768->current_volume < MAX9768_VOLUME_MIN)
		max9768->current_volume = MAX9768_VOLUME_MIN;

	msg = max9768->current_volume & MAX9768_VOLUME_MASK;
	pr_debug("%s: 0x%02x\n", __FUNCTION__, msg);
	err = max9768_send(client, msg);
	if (err < 0) {
		dev_err(dev, "couldn't set volume\n");
	}

	return count;
}

static ssize_t max9768_get_mute(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);

	return sprintf(buf, "%d", max9768->mute_state);
}

static ssize_t max9768_set_mute(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);

	if (simple_strtol(buf, NULL, 10) == 0) 
		max9768_mute(max9768, 0);
	else
		max9768_mute(max9768, 1);

	return count;
}

static ssize_t max9768_get_shutdown(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);

	return sprintf(buf, "%d", max9768->shdn_state);
}

static ssize_t max9768_set_shutdown(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct max9768 *max9768 = i2c_get_clientdata(client);

	if (simple_strtol(buf, NULL, 10) == 0) 	
		max9768_shutdown(max9768, 0);
	else 
		max9768_shutdown(max9768, 1);

	return count;
}

static DEVICE_ATTR(shutdown, S_IWUSR | S_IRUGO, max9768_get_shutdown, max9768_set_shutdown);
static DEVICE_ATTR(mute, S_IWUSR | S_IRUGO, max9768_get_mute, max9768_set_mute);
static DEVICE_ATTR(volume, S_IWUSR | S_IRUGO, max9768_get_volume, max9768_set_volume);

static int max9768_create_sys_entries(struct i2c_client *client)
{
	int status = 0;

	if ((status = device_create_file(&client->dev, &dev_attr_volume))) {
		dev_warn(&client->dev, "Unable to create sysfs attribute for volume register\n");
		goto exit;
	}
	if ((status = device_create_file(&client->dev, &dev_attr_mute))) {
		dev_warn(&client->dev, "Unable to create sysfs attribute for mute register\n");
		goto exit;
	}
	if ((status = device_create_file(&client->dev, &dev_attr_shutdown))) {
		dev_warn(&client->dev, "Unable to create sysfs attribute for shutdown register\n");
		goto exit;
	}

exit:
	return status;
}

static void max9768_remove_sys_entries(struct i2c_client *client)
{
	device_remove_file(&client->dev, &dev_attr_volume);
	device_remove_file(&client->dev, &dev_attr_mute);
	device_remove_file(&client->dev, &dev_attr_shutdown);
}


static int max9768_probe(struct i2c_client *client,
				 const struct i2c_device_id *id)
{
	struct max9768 *max9768;
	struct max9768_platform_data *platform_data; 
	int result;

	platform_data = (struct max9768_platform_data *)(client->dev.platform_data);
	if (!platform_data) {
		dev_err(&client->dev, "can't get the platform data for max9768\n");
		return -EINVAL;
	}

	max9768 = kzalloc(sizeof(struct max9768), GFP_KERNEL);
	if (!max9768) {
		dev_err(&client->dev, "can't allocate memory\n");
		return -ENOMEM;
	}

	max9768->client = client;
	i2c_set_clientdata(client, max9768);
	dev_set_drvdata(&client->dev, max9768);

	/* create /sys entries */
	result = max9768_create_sys_entries(client);
	if (result)
		goto out_free;

	if (platform_data->init) {
		result = platform_data->init();
		if (result) {
			dev_err(&client->dev, "can't reserve gpios\n");
			goto out_sys;
		}
	}
	max9768->mute_pin = platform_data->mute_pin;
	max9768->shdn_pin = platform_data->shdn_pin;
		
	/* set filter mode */
	result = max9768_send(client, platform_data->filter_mode);
	if (result < 0) {
		dev_err(&client->dev, "can't communicate with chip\n");
		goto out_gpio;
	}

	/* shutdown amplifier and activate mute */
	max9768_shutdown(max9768, 1);
	max9768_mute(max9768, 1);

	dev_notice(&client->dev, "successfully probed !\n");

	return 0;

out_gpio:
	if (platform_data->exit)
		platform_data->exit();
out_sys:
	max9768_remove_sys_entries(client);
out_free:
	i2c_set_clientdata(client, NULL);
	kfree(max9768);

	return result;
}

static int max9768_remove(struct i2c_client *client)
{
	struct max9768 *max9768 = i2c_get_clientdata(client);
	struct max9768_platform_data *pdata = client->dev.platform_data;

	max9768_remove_sys_entries(client);

	if (pdata->exit)
		pdata->exit();

	kfree(i2c_get_clientdata(client));
	kfree(max9768);

	return 0;
}

#ifdef CONFIG_PM
static int max9768_suspend(struct i2c_client *client, pm_message_t state)
{
	struct max9768 *max9768 = i2c_get_clientdata(client);

	gpio_set_value(max9768->mute_pin, 1);
	gpio_set_value(max9768->shdn_pin, 0);

	return 0;
}

static int max9768_resume(struct i2c_client *client)
{
	struct max9768 *max9768 = i2c_get_clientdata(client);

	/* reactivate the component just like before suspend */
	max9768_shutdown(max9768, max9768->shdn_state);
	max9768_mute(max9768, max9768->mute_state);

	return 0;
}
#else
#define max9768_suspend	NULL
#define max9768_resume	NULL
#endif


static const struct i2c_device_id max9768_id[] = {
	{ DRIVER_NAME, 0 },
	{},
};

static struct i2c_driver max9768_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = max9768_probe,
	.remove = max9768_remove,
	.suspend = max9768_suspend,
	.resume = max9768_resume,
	.id_table = max9768_id,
};

static int __init max9768_init(void)
{
	int ret;

	printk(DRIVER_NAME " v" DRIVER_VERSION "\n");
	ret = i2c_add_driver(&max9768_driver);
	if (ret)
		printk(KERN_ERR "unable to register to I2C bus !\n");

	return ret;
}
module_init(max9768_init);

static void __exit max9768_exit(void)
{
	i2c_del_driver(&max9768_driver);
}
module_exit(max9768_exit);

MODULE_AUTHOR("Nicolas Colombain <nicolas.colombain@armadeus.com>");
MODULE_DESCRIPTION("MAX9768 audio amplifier driver");
MODULE_LICENSE("GPL");

