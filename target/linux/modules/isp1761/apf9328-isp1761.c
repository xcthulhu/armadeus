#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#include <asm/hardware.h>
#include <asm/arch/imx-regs.h>
#else
#include <mach/hardware.h>
#include <mach/imx-regs.h>
#endif
#include <asm/delay.h>
#include <linux/usb/isp116x.h>

#define ISP1761_BASE       IMX_CS3_PHYS
#define ISP1761_GPIO_IRQ   GPIO_PORTC | 10
#define ISP1761_IRQ        IRQ_GPIOC(10) 

#define CS3U 0x18  /*chip select 3 upper register */
#define CS3L 0x1C  /*chip select 3 lower register */

#define CONFIG_SYS_BUS_FREQ 	96	/* 96|48... MHz (BCLOCK and HCLOCK) */
#define CFG_HCLK_LGTH		(1000/CONFIG_SYS_BUS_FREQ)	/* ns */



/* CS3 configuration for ISP176x USB Host */
#define CFG_CS3_CHIP_SELECT_ENABLE	1	/* 1 : enable CS0 peripherals  */
#define CFG_CS3_PIN_ASSERT		0	/* chip select pin state when */
						/* chip select disabled  */
#define CFG_CS3_DATA_PORT_SIZE		0x5	/* 5=16 bits on D[15:0] pins */
						/* 3=8bits on D[7:0] 6=32 bits..*/
#define CFG_CS3_SUPERVISOR_PROTECT	0	/* 1 : user mode access prohibited*/
#define CFG_CS3_WRITE_PROTECT		0	/* 1 : write access prohibited  */
#define CFG_CS3_EB_SIGNAL_CONTROL_WRITE	1	/* 1 when EB is used as write signal*/

#define CFG_CS3_READ_CYC_LGTH		105	/* ns */
#define CFG_CS3_OE_ASSERT_DLY	35		/* ns */
#define CFG_CS3_OE_NEG_DLY		35	/* ns */

#define CFG_CS3_CS_NEG_LGTH 	30	/* max 30 ns CS HIGH to CS LOW at 100MHz*/
#define CFG_CS3_XTRA_DEAD_CYC	0	/* ns from CS HIGH to tristate bus */

#define CFG_CS3_WRITE_XTRA_LGTH		0	/* ns */
#define CFG_CS3_EB_ASSERT_DLY		5	/* ns */
#define CFG_CS3_EB_NEG_DLY		65	/* ns */
#define CFG_CS3_CS_ASSERT_NEG_DLY	0	/* ns */

#define CFG_CS3_SYNC_ENABLE		0	/* enable synchronous burst mode*/
#define CFG_CS3_SYNC_PAGE_MODE_EMUL	0	/* enable page mode emulation */
#define CFG_CS3_SYNC_PAGE_SIZE		0	/* 4 words page size (8bytes) */
#define CFG_CS3_SYNC_BURST_CLK_START	0	/* 0 ns burst clock delay */
#define CFG_CS3_SYNC_BURST_CLK_DIV	0	/* 0 : divider is 1 */
#define CFG_CS3_SYNC_DAT_OUT_LGTH	0	/* ns */


#define CFG_CS3U_VAL\
	((((CFG_CS3_XTRA_DEAD_CYC+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)\
	|((((CFG_CS3_WRITE_XTRA_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<4)\
	|(((((CFG_CS3_READ_CYC_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)-1)&0x3F)<<8)\
	|((((CFG_CS3_CS_NEG_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x03)<<14)\
	|((((CFG_CS3_SYNC_DAT_OUT_LGTH+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|((CFG_CS3_SYNC_ENABLE&0x01)<<20)\
	|((CFG_CS3_SYNC_PAGE_MODE_EMUL&0x01)<<21)\
	|((CFG_CS3_SYNC_PAGE_SIZE&0x03)<<22)\
	|((((CFG_CS3_SYNC_BURST_CLK_START+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|((CFG_CS3_SYNC_BURST_CLK_DIV&0x03)<<28))

#define CFG_CS3L_VAL\
	((CFG_CS3_CHIP_SELECT_ENABLE&0x01)\
	|((CFG_CS3_PIN_ASSERT&0x01)<<1)\
	|((CFG_CS3_WRITE_PROTECT&0x01)<<4)\
	|((CFG_CS3_SUPERVISOR_PROTECT&0x01)<<6)\
	|((CFG_CS3_DATA_PORT_SIZE&0x07)<<8)\
	|((CFG_CS3_EB_SIGNAL_CONTROL_WRITE&0x01)<<11)\
	|((((CFG_CS3_CS_ASSERT_NEG_DLY+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<12)\
	|(((((CFG_CS3_EB_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<16)\
	|(((((CFG_CS3_EB_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<20)\
	|(((((CFG_CS3_OE_NEG_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<24)\
	|(((((CFG_CS3_OE_ASSERT_DLY*2)+CFG_HCLK_LGTH-1)/CFG_HCLK_LGTH)&0x0F)<<28))



static void apf_isp116x_delay(struct device *dev, int delay)
{
	/* On this platform, we work with 200MHz clock, giving
	   5 ns per instruction. The cycle below involves 2
	   instructions and we lose 2 more instruction times due
	   to pipeline flush at jump. I.e., we consume 20 ns
	   per cycle.
	 */
	int cyc = delay / 20;

	__asm__ volatile ("0:\n"
			  "     subs  %0, %1, #1\n"
			  "     bge   0b\n"
			  :"=r" (cyc)
			  :"0"(cyc));
}

static struct isp116x_platform_data isp116x_pdata = {
	.sel15Kres              = 1,
	.oc_enable              = 1,
	.int_act_high           = 0,
	.int_edge_triggered     = 1,
	.remote_wakeup_enable   = 1,
	.delay                  = apf_isp116x_delay,
};

static struct resource devfull_isp1761_resources[] = {
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

static struct platform_device devfull_isp1761_device = {
	.name           = "isp1761",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(devfull_isp1761_resources),
	.resource       = devfull_isp1761_resources,
    .dev            = { .platform_data = &isp116x_pdata, },
};

static struct platform_device *devfull_isp1761_devices[] = {
	&devfull_isp1761_device,
};

int __init devfull_isp1761_init(void)
{
	unsigned int num_devices=ARRAY_SIZE(devfull_isp1761_devices);
    /* configure the chip select 3 */
    __REG(IMX_EIM_BASE + CS3U + IMX_IO_BASE) = CFG_CS3U_VAL; //CSU
    __REG(IMX_EIM_BASE + CS3L + IMX_IO_BASE) = CFG_CS3L_VAL; //CSL
    imx_gpio_mode( ISP1761_GPIO_IRQ | GPIO_IN | GPIO_GIUS ); /* Set up IRQ line. */
	set_irq_type( ISP1761_IRQ, IRQF_TRIGGER_FALLING );
	
	return platform_add_devices(devfull_isp1761_devices, num_devices);
}

void __exit devfull_isp1761_exit(void)
{
	platform_device_unregister(&devfull_isp1761_device);
}

module_init(devfull_isp1761_init);
module_exit(devfull_isp1761_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain - Armadeus systems");
MODULE_DESCRIPTION("APF9328 ISP1761 chip initialization driver");
MODULE_VERSION("0.2");
