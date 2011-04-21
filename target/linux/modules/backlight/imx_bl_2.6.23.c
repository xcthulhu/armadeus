/*
 *  Backlight Driver for i.MX based platforms
 *
 *  Copyright (c) 2007 Julien Boibessot - Armadeus Project
 *
 *  Based on Backlight Driver for Sharp Zaurus Handhelds
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
#include <linux/version.h>
#include <mach/imxfb.h>        /* Backlight machinfo struct is defined here */
#include <mach/hardware.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
#ifdef CONFIG_ARCH_IMX
#define LCDC_BASE_ADDR 		IMX_LCDC_BASE
#endif /* CONFIG_ARCH_IMX */
#define MXC_IO_ADDRESS		IO_ADDRESS
#else
#ifdef CONFIG_ARCH_MX1
#include <mach/mx1.h>
#define LCDC_BASE_ADDR		MX1_LCDC_BASE_ADDR
#define MXC_IO_ADDRESS		MX1_IO_ADDRESS
#else
# include <mach/mx2x.h>
#define LCDC_BASE_ADDR		MX27_LCDC_BASE_ADDR
#define MXC_IO_ADDRESS		MX27_IO_ADDRESS
#endif /* CONFIG_ARCH_MX1 */
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38) */

#define DRIVER_NAME    "imx-bl"
#define DRIVER_VERSION "0.2"

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
	int intensity = bd->props.brightness;
	unsigned long shadow = 0;

	if (bd->props.power != FB_BLANK_UNBLANK)
		intensity = 0;
	if (bd->props.fb_blank != FB_BLANK_UNBLANK)
		intensity = 0;
	if (imxbl_flags & IMXBL_SUSPENDED)
		intensity = 0;
	if (imxbl_flags & IMXBL_BATTLOW)
		intensity &= bl_machinfo->limit_mask;

	/* If a method was given in machine info then use it */
	if( bl_machinfo->set_bl_intensity ) {
		bl_machinfo->set_bl_intensity(intensity);
	} else { /* Otherwise use this default one: */
		shadow = readl(MXC_IO_ADDRESS(LCDC_BASE_ADDR) + 0x2c); /* PWMR / LPCCR */
		shadow &= ~PWMR_PW(0xff);
		shadow |= PWMR_PW(intensity);
		writel(shadow, MXC_IO_ADDRESS(LCDC_BASE_ADDR) + 0x2c);
		pr_debug("Setting backlight intensity to %d\n", intensity);
	}

	imxbl_intensity = intensity;

	return 0;
}

#ifdef CONFIG_PM
static int imxbl_suspend(struct platform_device *dev, pm_message_t state)
{
	struct backlight_device *bd = platform_get_drvdata(dev);

	imxbl_flags |= IMXBL_SUSPENDED;
	backlight_update_status(bd);
	return 0;
}

static int imxbl_resume(struct platform_device *dev)
{
	struct backlight_device *bd = platform_get_drvdata(dev);

	imxbl_flags &= ~IMXBL_SUSPENDED;
	backlight_update_status(bd);
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
	backlight_update_status(imx_backlight_device);
}
EXPORT_SYMBOL(imxbl_limit_intensity);


static struct backlight_ops imxbl_ops = {
	.get_brightness = imxbl_get_intensity,
	.update_status  = imxbl_send_intensity,
};

static int imxbl_probe(struct platform_device *pdev)
{
	struct imxbl_machinfo *machinfo = pdev->dev.platform_data;

	bl_machinfo = machinfo;
	imxbl_data.max_brightness = machinfo->max_intensity;
	if (!machinfo->limit_mask)
		machinfo->limit_mask = -1;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38)
	struct backlight_properties props;

	memset(&props, 0, sizeof(struct backlight_properties));
	props.max_brightness = machinfo->max_intensity;
	props.power = FB_BLANK_UNBLANK;
	props.brightness = machinfo->default_intensity;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
	imx_backlight_device = backlight_device_register(DRIVER_NAME,
								&pdev->dev, NULL, &imxbl_ops);
#else
	imx_backlight_device = backlight_device_register(DRIVER_NAME,
								&pdev->dev, NULL, &imxbl_ops, &props);
#endif

	if (IS_ERR (imx_backlight_device)) {
		printk("can't register backlight device\n");
		return PTR_ERR (imx_backlight_device);
	}

	platform_set_drvdata(pdev, imx_backlight_device);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
	imx_backlight_device->props.max_brightness = machinfo->max_intensity;
	imx_backlight_device->props.power = FB_BLANK_UNBLANK;
	imx_backlight_device->props.brightness = machinfo->default_intensity;
#endif

	backlight_update_status(imx_backlight_device);

	printk("i.MX Backlight driver v" DRIVER_VERSION " initialized.\n");
	return 0;
}

static int imxbl_remove(struct platform_device *pdev)
{
	struct backlight_device *bd = platform_get_drvdata(pdev);

	imxbl_data.power = 0;
	imxbl_data.brightness = 0;
	backlight_update_status(bd);

	backlight_device_unregister(bd);

	printk("i.MX Backlight driver unloaded\n");
	return 0;
}

static struct platform_driver imxbl_driver = {
	.probe		= imxbl_probe,
	.remove		= imxbl_remove,
	.suspend	= imxbl_suspend,
	.resume		= imxbl_resume,
	.driver		= {
		.name	= DRIVER_NAME,
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
MODULE_DESCRIPTION("i.MX Backlight Driver");
MODULE_LICENSE("GPL");
