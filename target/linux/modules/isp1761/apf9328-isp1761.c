/*
 * Deprecated - Now part of apf9328.c kernel file and U-Boot 1.3.4 patch 3.5
 */

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <linux/version.h>
#include <mach/hardware.h>
#include <mach/imx-regs.h>


int __init devfull_isp1761_init(void)
{	
	return 0;
}

void __exit devfull_isp1761_exit(void)
{
}

module_init(devfull_isp1761_init);
module_exit(devfull_isp1761_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain - Armadeus systems");
MODULE_DESCRIPTION("APF9328 ISP1761 chip initialization driver");
MODULE_VERSION("0.2");
