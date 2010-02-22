/*
 *  Display Driver for hx5116 controler for AMOLED 
 *
 *  Copyright (c) 2009 Fabien Marteau - ARMadeus Systems
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
#include <linux/delay.h>

#include <linux/display.h>
#include <linux/spi/spi.h>

#include <linux/version.h>
#include <mach/imxfb.h>        /* Backlight machinfo struct is defined here */
#include <mach/hardware.h>

#include <video/hx5116.h>

#define DRIVER_NAME    "hx5116-display"
#define DRIVER_VERSION "0.3"

/* write value hx5116 display register via SPI */
void hx5116_write(struct hx5116_display *hx5116_dev, u8 address, u16 data)
{
	u8 command[2];
	int ret;

	/* Address */
	command[0] = (address << 1) | HX5116_SPI_WRITE;
	command[1] = data;
	ret = spi_write(hx5116_dev->spi, command, 2);
	if (ret < 0)
		printk("ERROR: can't write on hx5116\n");
}

/* XXX: not functionnal, need half duplex capability under spi_gpio driver */
u8 hx5116_read(struct hx5116_display *hx5116_dev, u8 address)
{
	u8 command;
	ssize_t ret;

	/* Address */
	command = (address << 1) | HX5116_SPI_READ;
	ret = spi_w8r8(hx5116_dev->spi, command);
	if (ret < 0)
		printk("ERROR: can't read on hx5116\n");

	return (u8)ret;
}

static int hx5116_display_get_contrast(struct display_device *dev)
{
	struct hx5116_display *hx5116_dev = dev->priv_data;

	/* TODO: read register value with spi_w8r8 */

	return hx5116_dev->gcontrast_value;
}

static int hx5116_display_set_contrast(struct display_device *dev, unsigned int value)
{
	struct hx5116_display *hx5116_dev = dev->priv_data;
	
	hx5116_write(hx5116_dev, HX5116_RGB_CONTRAST, value);
	hx5116_dev->gcontrast_value = value;

	return value;
}

static int hx5116_display_probe(struct display_device *dev, void *data)
{
	return 1;
}

static int  hx5116_display_remove(struct display_device *dev)
{
	return 0;
}

#ifdef CONFIG_PM
static void hx5116_display_suspend(struct display_device *dev, pm_message_t state)
{
	struct hx5116_display *hx5116_dev = dev->priv_data;
}

static void hx5116_display_resume(struct display_device *dev)
{
	struct hx5116_display *hx5116_dev = dev->priv_data;
}
#else
#define hx5116_display_suspend	NULL
#define hx5116_display_resume	NULL
#endif /* CONFIG_PM */

static struct display_driver hx5116_ops = {
	.get_contrast 	= hx5116_display_get_contrast,
	.set_contrast 	= hx5116_display_set_contrast,
	.probe       	= hx5116_display_probe,
	.suspend	= hx5116_display_suspend,
	.resume		= hx5116_display_resume,
	.remove		= hx5116_display_remove,
	.max_contrast	= 63,
};


static void hx5116_init_sequence(struct hx5116_display *hx5116_dev)
{
	hx5116_dev->reset_on(0); /* reset off */

	/* initial procedure for 24-bit parallel RGB (DE) */
	hx5116_write (hx5116_dev, 0x04, 0x23); /*set display mode 24-bit parallel RGB (DE)*/
	hx5116_write (hx5116_dev, 0x05, 0x82); /*set display mode*/
	hx5116_write (hx5116_dev, 0x07, 0x0F); /*set driver capability*/
	hx5116_write (hx5116_dev, 0x34, 0x18); /*set display timing*/
	hx5116_write (hx5116_dev, 0x35, 0x28); /*set display timing*/
	hx5116_write (hx5116_dev, 0x36, 0x16); /*set display timing*/
	hx5116_write (hx5116_dev, 0x37, 0x01); /*set display timing*/
	hx5116_write (hx5116_dev, 0x02, 0x02); /*OTP On*/
	hx5116_write (hx5116_dev, 0x0A, 0x79); /*VGHVGL=+/-6V*/
	hx5116_write (hx5116_dev, 0x09, 0x20); /*VGAM1OUT=4.85V*/
	hx5116_write (hx5116_dev, 0x10, 0x6A); /*set R slop*/
	hx5116_write (hx5116_dev, 0x11, 0x6A); /*set G slop*/
	hx5116_write (hx5116_dev, 0x12, 0x68); /*set B slop*/
	hx5116_write (hx5116_dev, 0x13, 0x00); /*set R_0*/
	hx5116_write (hx5116_dev, 0x14, 0x04); /*set R_10*/
	hx5116_write (hx5116_dev, 0x15, 0x05); /*set R_36*/
	hx5116_write (hx5116_dev, 0x16, 0x05); /*set R_80*/
	hx5116_write (hx5116_dev, 0x17, 0x04); /*set R_124*/
	hx5116_write (hx5116_dev, 0x18, 0x03); /*set R_168*/
	hx5116_write (hx5116_dev, 0x19, 0x02); /*set R_212*/
	hx5116_write (hx5116_dev, 0x1A, 0x02); /*set R_255*/
	hx5116_write (hx5116_dev, 0x1B, 0x00); /*set G_0*/
	hx5116_write (hx5116_dev, 0x1C, 0x06); /*set G_10*/
	hx5116_write (hx5116_dev, 0x1D, 0x05); /*set G_36*/
	hx5116_write (hx5116_dev, 0x1E, 0x06); /*set G_80*/
	hx5116_write (hx5116_dev, 0x1F, 0x06); /*set G_124*/
	hx5116_write (hx5116_dev, 0x20, 0x05); /*set G_168*/
	hx5116_write (hx5116_dev, 0x21, 0x05); /*set G_212*/
	hx5116_write (hx5116_dev, 0x22, 0x07); /*set G_255*/
	hx5116_write (hx5116_dev, 0x23, 0x00); /*set G_0*/
	hx5116_write (hx5116_dev, 0x24, 0x07); /*set B_10*/
	hx5116_write (hx5116_dev, 0x25, 0x06); /*set B_36*/
	hx5116_write (hx5116_dev, 0x26, 0x07); /*set B_80*/
	hx5116_write (hx5116_dev, 0x27, 0x07); /*set B_124*/
	hx5116_write (hx5116_dev, 0x28, 0x06); /*set B_168*/
	hx5116_write (hx5116_dev, 0x29, 0x04); /*set B_212*/
	hx5116_write (hx5116_dev, 0x2A, 0x07); /*set B_255*/
	hx5116_write (hx5116_dev, 0x06, 0x03); /*set display on*/
}

static int hx5116_spi_probe(struct spi_device *pdev)
{
	int err = -EINVAL;
	struct display_device *disp_dev;
	struct hx5116_display *hx5116_dev = pdev->dev.platform_data;

	/* fill hx5116 display structure */
	disp_dev = display_device_register(&hx5116_ops,
					&pdev->dev,
					hx5116_dev);
	if (disp_dev == NULL)
		goto display_device_error;
	disp_dev->priv_data = hx5116_dev;
	hx5116_dev->display_dev = disp_dev;
	hx5116_dev->spi = pdev;

	/* configure spi */
	hx5116_dev->spi->mode = SPI_MODE_0;
	hx5116_dev->spi->bits_per_word = 8;
	hx5116_dev->spi->dev.power.power_state = PMSG_ON;
	err = spi_setup(hx5116_dev->spi);
	if (err < 0) {
		goto spi_error;
	}

	hx5116_init_sequence(hx5116_dev);

	return 0;

spi_error:
	printk("ERROR: can't setup spi for hx5116\n");
	display_device_unregister(hx5116_dev->display_dev);
display_device_error:
	printk("ERROR: can't register display device\n");
	return err;
}

static int hx5116_spi_remove(struct spi_device *pdev)
{
	struct hx5116_display *hx5116_dev = pdev->dev.platform_data;

	hx5116_dev->reset_on(1); /* let hx5116 in reset */
	display_device_unregister(hx5116_dev->display_dev);

	return 0;
}

#ifdef CONFIG_PM
static int hx5116_spi_suspend(struct spi_device *dev, pm_message_t state)
{
	return 0;
}

static int hx5116_spi_resume(struct spi_device *dev)
{
	return 0;
}
#else
#define hx5116_suspend	NULL
#define hx5116_resume	NULL
#endif

static void hx5116_spi_shutdown(struct spi_device *spi)
{

}

static struct spi_driver hx5116_driver = {
	.probe		= hx5116_spi_probe,
	.remove		= hx5116_spi_remove,
	.shutdown	= hx5116_spi_shutdown,
	.suspend	= hx5116_spi_suspend,
	.resume		= hx5116_spi_resume,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init hx5116_init(void)
{
	return spi_register_driver(&hx5116_driver);
}

static void __exit hx5116_exit(void)
{
	spi_unregister_driver(&hx5116_driver);
}

module_init(hx5116_init);
module_exit(hx5116_exit);

MODULE_AUTHOR("Fabien Marteau <fabien.marteau@armadeus.com");
MODULE_DESCRIPTION("HX5116 AMOLED Driver");
MODULE_LICENSE("GPL");

