/*
 *  Backlight Driver for i.MXL based platform 
 *
 *  Copyright (c) 2007 Armadeus Project
 *
 *  Based on Sharp's 2.4 Backlight Driver
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <asm/arch/imxfb.h> // Backlight machinfo struct is defined here
#include <asm/arch/imx-regs.h>


static int imxbl_intensity;
static DEFINE_MUTEX(bl_mutex);
static struct backlight_properties imxbl_data;
static struct backlight_device *imx_backlight_device;
static struct imxbl_machinfo *bl_machinfo;

static unsigned long imxbl_flags;
#define IMXBL_SUSPENDED     0x01
#define IMXBL_BATTLOW       0x02

static int imxbl_send_intensity(struct backlight_device *bd)
{
	int intensity = bd->props->brightness;
    unsigned long shadow = 0;

	if (bd->props->power != FB_BLANK_UNBLANK)
		intensity = 0;
	if (bd->props->fb_blank != FB_BLANK_UNBLANK)
		intensity = 0;
	if (imxbl_flags & IMXBL_SUSPENDED)
		intensity = 0;
	if (imxbl_flags & IMXBL_BATTLOW)
		intensity &= bl_machinfo->limit_mask;

 	mutex_lock(&bl_mutex);
    if( bl_machinfo->set_bl_intensity ) {
        bl_machinfo->set_bl_intensity(intensity);
    } else {
        shadow = LCDC_PWMR;
        shadow &= 0xffffff00;
        shadow |= PWMR_PW(intensity);
        LCDC_PWMR = shadow;
        printk("Setting backlight intensity to %d\n", intensity);
    }
	mutex_unlock(&bl_mutex);

	imxbl_intensity = intensity;

	return 0;
}

#ifdef CONFIG_PM
static int imxbl_suspend(struct platform_device *dev, pm_message_t state)
{
	imxbl_flags |= IMXBL_SUSPENDED;
	imxbl_send_intensity(imx_backlight_device);
	return 0;
}

static int imxbl_resume(struct platform_device *dev)
{
	imxbl_flags &= ~IMXBL_SUSPENDED;
	imxbl_send_intensity(imx_backlight_device);
	return 0;
}
#else
#define imxbl_suspend	NULL
#define imxbl_resume	NULL
#endif

static int imxbl_get_intensity(struct backlight_device *bd)
{
	return imxbl_intensity;
}

static int imxbl_set_intensity(struct backlight_device *bd)
{
	imxbl_send_intensity(imx_backlight_device);
	return 0;
}

/*
 * Called when the battery is low to limit the backlight intensity.
 * If limit==0 clear any limit, otherwise limit the intensity
 */
void imxbl_limit_intensity(int limit)
{
	if (limit)
		imxbl_flags |= IMXBL_BATTLOW;
	else
		imxbl_flags &= ~IMXBL_BATTLOW;
	imxbl_send_intensity(imx_backlight_device);
}
EXPORT_SYMBOL(imxbl_limit_intensity);


static struct backlight_properties imxbl_data = {
	.owner          = THIS_MODULE,
	.get_brightness = imxbl_get_intensity,
	.update_status  = imxbl_set_intensity,
};

static int imxbl_probe(struct platform_device *pdev)
{
	struct imxbl_machinfo *machinfo = pdev->dev.platform_data;

	bl_machinfo = machinfo;
	imxbl_data.max_brightness = machinfo->max_intensity;
	if (!machinfo->limit_mask)
		machinfo->limit_mask = -1;

	imx_backlight_device = backlight_device_register ("imxl-bl",
		NULL, &imxbl_data);
	if (IS_ERR (imx_backlight_device))
    {
        printk("Error imxBL\n");
		return PTR_ERR (imx_backlight_device);
    }

	imxbl_data.power = FB_BLANK_UNBLANK;
	imxbl_data.brightness = machinfo->default_intensity;
	imxbl_send_intensity(imx_backlight_device);

	printk("i.MXL Backlight Driver Initialized.\n");
	return 0;
}

static int imxbl_remove(struct platform_device *dev)
{
	backlight_device_unregister(imx_backlight_device);

	printk("i.MXL Backlight Driver Unloaded\n");
	return 0;
}

static struct platform_driver imxbl_driver = {
	.probe		= imxbl_probe,
	.remove	= imxbl_remove,
	.suspend	= imxbl_suspend,
	.resume	= imxbl_resume,
	.driver		= {
		.name	= "imxl-bl",
	},
};

static int __init imxbl_init(void)
{
    return platform_driver_register(&imxbl_driver);
}

static void __exit imxbl_exit(void)
{
    platform_driver_unregister(&imxbl_driver);
}

module_init(imxbl_init);
module_exit(imxbl_exit);

MODULE_AUTHOR("Julien Boibessot <julien.boibessot@armadeus.com>");
MODULE_DESCRIPTION("i.MXL Backlight Driver");
MODULE_LICENSE("GPL");
