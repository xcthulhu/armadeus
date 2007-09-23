#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <asm/hardware.h>
#include <asm/arch/imx-regs.h>
#include <asm/delay.h>

//#include <asm/arch/gumstix.h>

#define ISP1761_BASE       IMX_CS2_PHYS
#define ISP1761_GPIO_RESET 52
#define ISP1761_GPIO_IRQ   11
#define ISP1761_IRQ        IRQ_GPIOD(ISP1761_GPIO_IRQ) // ???
#define ISP1761_GPIO_CS    666 // ??? GPIO78_nCS_2_MD

//
// !! A terme ce fichier devra être inclu dans notre apf9328.c mais pour l'instant laisse le là Nico !!
//

static struct resource m6_isp1761_resources[] = {
	[0] = {
		.name	= "isp1761-regs",
		.start  = ISP1761_BASE + 0x00000000,
		.end    = ISP1761_BASE + 0x000fffff,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = ISP1761_IRQ,
		.end    = ISP1761_IRQ,
		.flags  = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWLEVEL,
	},
};

static struct platform_device m6_isp1761_device = {
	.name           = "isp1761",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(m6_isp1761_resources),
	.resource       = m6_isp1761_resources,
};

static struct platform_device *m6_isp1761_devices[] = {
	&m6_isp1761_device,
};

int __init m6_isp1761_init(void)
{
	unsigned int num_devices=ARRAY_SIZE(m6_isp1761_devices);

	imx_gpio_mode( GPIO_PORTD | 8 | GPIO_IN | GPIO_GIUS );                /* Set up nPWE */
	imx_gpio_mode( ISP1761_GPIO_CS );               /* Set up chip-select line. */
	imx_gpio_mode( ISP1761_GPIO_RESET | GPIO_OUT ); /* Set up reset line. */
	imx_gpio_mode( ISP1761_GPIO_IRQ   | GPIO_IN ); /* Set up IRQ line. */

	set_irq_type( ISP1761_IRQ, IRQF_TRIGGER_FALLING );
	
	/* Reset the chip (hold low for 1ms). */
// 	GPCR(ISP1761_GPIO_RESET) = GPIO_bit(ISP1761_GPIO_RESET);
// 	udelay(1000);
// 	GPSR(ISP1761_GPIO_RESET) = GPIO_bit(ISP1761_GPIO_RESET);

	return platform_add_devices(m6_isp1761_devices, num_devices);
}

void __exit m6_isp1761_exit(void)
{
	platform_device_unregister(&m6_isp1761_device);
}

module_init(m6_isp1761_init);
module_exit(m6_isp1761_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JB");
MODULE_DESCRIPTION("APF9328 ISP1761 chip initialization driver");
MODULE_VERSION("0.1");
