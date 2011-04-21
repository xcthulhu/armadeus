/*
 *  PS/2 keyboard controller driver for Armadeus APF9328 board with Spartan3 FPGA
 *
 *  Copyright (c) 2006 Armadeus Team
 *
 *  Based on the work of Vojtech Pavlik
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

/*#define DEBUG 1*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/kernel.h>	/* pr_debug */

#include <mach/imx-regs.h>

/* Linux Kernel uses virtual addresses linearly mapped to physical one, ex for CS1:
#define IMX_CS1_PHYS        0x12000000
#define IMX_CS1_SIZE        0x01000000
#define IMX_CS1_VIRT        0xea000000*/

/* Following defines will have to be put in a .h file somewhere when driver will be finished */
#define FPGA_PS2_STATUS_REGISTER (*((volatile unsigned short*)(IMX_CS1_VIRT + 0x00)))
#define FPGA_PS2_FIFO_FULL	(0x08)
#define FPGA_PS2_FIFO_EMPTY	(0x02)
#define FPGA_PS2_DATA_REGISTER	(*((volatile unsigned short*)(IMX_CS1_VIRT + 0x02)))
#define FPGA_PS2_ID_REGISTER	(*((volatile unsigned short*)(IMX_CS1_VIRT + 0x08)))
#define FPGA_PS2_ID_VALUE	(0x1234)

#define FPGA_PS2_DEFAULT_ADDRESS 0x12000000
/* Base address in i.MX memory map to access PS/2 interface on FPGA   NOT HANDLED YET, cf previous defines */
static unsigned int base_address = FPGA_PS2_DEFAULT_ADDRESS;
module_param_named(base, base_address, uint, 0);
MODULE_PARM_DESC(base, "Base address in i.MX memory map to access PS/2 interface on FPGA (default is 0x12000000)");

/* Compatibility mode for keyboard, I'm not sure we should support XT... */
static unsigned int apf9328keyboard_mode = SERIO_8042;
module_param_named(mode, apf9328keyboard_mode, uint, 0);
MODULE_PARM_DESC(mode, "Mode of operation: XT = 0/AT = 1 (default)");

/* Global variables */
static int gBuffer;
static struct serio *apf9328keyboard_port;
static struct timer_list read_timer;
#define DRIVER_NAME "Armadeus PS/2"
#define DRIVER_VERSION "0.2"


/* Read one entry in FPGA FIFO */
static int apf9328keyboard_read(void)
{
	volatile unsigned short value;

	value = FPGA_PS2_DATA_REGISTER;
	pr_debug(DRIVER_NAME ": value read 0x%x\n", value);

	return value;
}

/* Flush receive FIFO in FPGA */
static int apf9328keyboard_flush(void)
{
	volatile unsigned short shadow;
	unsigned int timeout = 0;

	while (((shadow & FPGA_PS2_FIFO_EMPTY) == 0) && (timeout <= 50))
	{
		shadow = FPGA_PS2_DATA_REGISTER;
		shadow = FPGA_PS2_STATUS_REGISTER;
		timeout++;
	}

	return 0;
}

#if 0
static int apf9328keyboard_write(struct serio *port, unsigned char c)
{
	/* Not implemented yet in hardware... */
	return 0;
}
#endif

/* Simulated by a software interrupt (timer) at that time... */
#ifdef HARDWARE_INTERRUPT_ACTIVATED
static int apf9328keyboard_interrupt(int irq, void *dev_id, struct pt_regs *regs)
#else
static void apf9328keyboard_interrupt(unsigned long arg)
#endif // HARDWARE_INTERRUPT_ACTIVATED
{
	volatile unsigned short status = 0;
	/*printk("RD IT: "); */

	status = FPGA_PS2_STATUS_REGISTER;
	/* Get data from FPGA (if there is some) */
	while ((status & FPGA_PS2_FIFO_EMPTY) == 0 )
	{
		gBuffer = apf9328keyboard_read();
		/* Apply it some processing */
		/* ...??? */
		/* Forward it to upper layer */
#ifdef HARDWARE_INTERRUPT_ACTIVATED
		serio_interrupt(apf9328keyboard_port, gBuffer, 0, regs);
		status = FPGA_PS2_STATUS_REGISTER;
	}

	return(IRQ_HANDLED);
#else 
		serio_interrupt(apf9328keyboard_port, gBuffer, 0);
		status = FPGA_PS2_STATUS_REGISTER;
		pr_debug(DRIVER_NAME ": status read 0x%x\n", status);
	}
	/* Trigger timer again */
	read_timer.expires = jiffies + HZ/20; /* Schedule next interrupt in 50 msec */
	add_timer(&read_timer);
#endif /* HARDWARE_INTERRUPT_ACTIVATED */
}

/* Check if FPGA was loaded with PS/2 IP */
static int apf9328keyboard_checkinterface(void)
{
	int status = 0;

	/* Try to access test register */
	if (FPGA_PS2_ID_REGISTER != FPGA_PS2_ID_VALUE) {
		status = -ENOMEM;
	} else {
		/* Flush Fifo */
		apf9328keyboard_flush();
	}

	return status;
}

/* Initialize structure to communicate with upper layer: input/serio */
static struct serio * __init apf9328keyboard_allocate_serio(void)
{
	struct serio *serio;
	
	serio = kmalloc(sizeof(struct serio), GFP_KERNEL);
	if (serio) {
		memset(serio, 0, sizeof(struct serio));
		serio->id.type = apf9328keyboard_mode;
	/*        serio->write = apf9328keyboard_write, UNCOMMENT THIS ONLY IF YOU WANT TO HAVE HOST->KEYBOARD COMM (YOU WILL HAVE TO IMPLEMENTE WRITE) */
		strlcpy(serio->name, "APF9328 AT/XT keyboard adapter", sizeof(serio->name));
		strlcpy(serio->phys, "Spartan3 PS/2", sizeof(serio->phys));
	}
	
	return serio;
}

static int __init apf9328keyboard_init( void )
{
	int err;

	/* Check if FPGA was correctly loaded */
	err = apf9328keyboard_checkinterface();
	if (err) {
		printk(KERN_ERR DRIVER_NAME ": Unable to find PS/2 controller. This driver requires a board with a FPGA loaded with the right IP!\n" );
		return(err);
	}

	/* Register our driver to upper layer (input/serio) */
	apf9328keyboard_port = apf9328keyboard_allocate_serio();
	if (!apf9328keyboard_port) {
		return(-ENOMEM);
	}
	serio_register_port(apf9328keyboard_port);

	/* Initialize and trigger read timer (when no hardware interrupt is available on controller) */
	init_timer(&read_timer);
	read_timer.expires = jiffies + HZ; /* Schedule next interrupt in 1 sec */
	read_timer.data = 0;
	read_timer.function = apf9328keyboard_interrupt;
	add_timer(&read_timer);

	printk(KERN_INFO DRIVER_NAME " v" DRIVER_VERSION ": APF9328KBD %s adapter OK!\n", apf9328keyboard_mode ? "AT" : "XT");

	return 0;
}

static void __exit apf9328keyboard_exit( void )
{
	del_timer(&read_timer);
	serio_unregister_port(apf9328keyboard_port);
	/*kfree( apf9328keyboard_port ); ?? */
	printk( KERN_INFO DRIVER_NAME ": successfully unloaded\n" );
}


module_init(apf9328keyboard_init);
module_exit(apf9328keyboard_exit);

MODULE_AUTHOR("Julien Boibessot, Michael Lerjen");
MODULE_DESCRIPTION("Armadeus APF9328 PS/2 keyboard controller driver");
MODULE_LICENSE("GPL");
