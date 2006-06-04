/*
 *  Copyright (c) 2006 Eric Jarrige
 *
 * Based on LoCoMo keyboard driver
 */

/*
 * imxkeypad driver for Linux/ARM
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/config.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/irq.h>

MODULE_AUTHOR("Eric Jarrige <jorasse@users.sourceforge.net>");
MODULE_DESCRIPTION("IMX keypad driver");
MODULE_LICENSE("GPL");

#define KPD_ROW_IDX		0
#define KPD_COL_IDX		1

#define KPD_MAX_ROWS		8
#define KPD_ROWS		4
#define KPD_ROWS_BIT_OFFSET	3

#define KPD_MAX_COLS		16
#define KPD_COLS		4
#define KPD_COLS_BIT_OFFSET	7

#define IMXKEYPAD_NUMKEYS 	(KPD_ROWS * KPD_COLS)
#define KB_ROWMASK(r)		(1 << (r + KPD_ROWS_BIT_OFFSET))
#define SCANCODE(c,r)		( (c * KPD_ROWS) + (r) )

#define KB_DELAY		8
#define SCAN_INTERVAL		(HZ/10)

static unsigned char imxkeypad_keycode[IMXKEYPAD_NUMKEYS] = {
	KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_ESC,	/* 0 - 3 */
	KEY_3, KEY_6, KEY_9, KEY_SPACE,			/* 4 - 7 */
	KEY_2, KEY_5, KEY_8, KEY_0,			/* 8 - 11 */
	KEY_1, KEY_4, KEY_7, KEY_KPASTERISK		/* 12 - 15 */
};

struct imxkeypad {
	unsigned char keycode[IMXKEYPAD_NUMKEYS];
	struct input_dev input;
	char phys[32];

	unsigned long base;
	int num_rows;
	int rows_offset;
	int num_cols;
	int cols_offset;

	spinlock_t lock;
	
	struct timer_list timer;
	char	irq_disabled;
};

static int	irq[KPD_MAX_ROWS] = {
	IRQ_GPIOA(0+KPD_ROWS_BIT_OFFSET),
	IRQ_GPIOA(1+KPD_ROWS_BIT_OFFSET),
	IRQ_GPIOA(2+KPD_ROWS_BIT_OFFSET),
	IRQ_GPIOA(3+KPD_ROWS_BIT_OFFSET)
};
static unsigned int	num_irq = KPD_ROWS;
module_param_array(irq, int, &num_irq, 0000);
MODULE_PARM_DESC(irq, "irqs asserted on key pressed");


static long int io_addr[]={(long)&DR(0),(long)&SSR(0)};
module_param_array(io_addr, long, NULL, 0000);
MODULE_PARM_DESC(io_addr, "control and status register addresses");
#define CTRL_REG (io_addr[0])
#define STAT_REG (io_addr[1])


static long int size[]={4,4};
module_param_array(size, long, NULL, 0000);
MODULE_PARM_DESC(size, "matrix size: num rows, num cols");

static long int bit_offset[]={3,7};
module_param_array(bit_offset, long, NULL, 0000);
MODULE_PARM_DESC(bit_offset, "rows and cols bits offset in registers");

/* helper functions for reading the keyboard matrix */
static inline void imxkeypad_charge_all(unsigned long membase)
{
	DR(0) |= (((1<<KPD_COLS)-1)<<KPD_COLS_BIT_OFFSET);
}

static inline void imxkeypad_activate_all(unsigned long membase)
{
	DR(0) &= ~(((1<<KPD_COLS)-1)<<KPD_COLS_BIT_OFFSET);
}

static inline void imxkeypad_activate_col(unsigned long membase, int col)
{
	DR(0) &= ~(1 << (col + KPD_COLS_BIT_OFFSET));
}

static inline void imxkeypad_reset_col(unsigned long membase, int col)
{
	DR(0) |= (1 << (col + KPD_COLS_BIT_OFFSET));
}

/*
 * The LoCoMo keyboard only generates interrupts when a key is pressed.
 * So when a key is pressed, we enable a timer.  This timer scans the
 * keyboard, and this is how we detect when the key is released.
 */

/* Scan the hardware keyboard and push any changes up through the input layer */
static void imxkeypad_scankeyboard(struct imxkeypad *imxkeypad, struct pt_regs *regs) 
{
	unsigned int row, col, rowd, scancode;
	unsigned long flags;
	unsigned int num_pressed;
	unsigned long membase = imxkeypad->base;
	
	spin_lock_irqsave(&imxkeypad->lock, flags);

	if (!imxkeypad->irq_disabled) {
		for (row = 0; row < num_irq; row++) {
			disable_irq_nosync(irq[row]);
		}
		imxkeypad->irq_disabled = 1;
	}
	//disable_irq_nosync(irq);


	if (regs)
		input_regs(&imxkeypad->input, regs);
	
	imxkeypad_charge_all(membase);

	num_pressed = 0;
	for (col = 0; col < KPD_COLS; col++) {

		imxkeypad_activate_col(membase, col);
		udelay(KB_DELAY);
		 
		rowd = ~readl(&SSR(0));
		for (row = 0; row < KPD_ROWS; row++ ) {
			scancode = SCANCODE(col, row);
			if (rowd & KB_ROWMASK(row)) {
				num_pressed += 1;
				input_report_key(&imxkeypad->input, imxkeypad->keycode[scancode], 1);
			} else {
				input_report_key(&imxkeypad->input, imxkeypad->keycode[scancode], 0);
			}
		}
		imxkeypad_reset_col(membase, col);
	}
	imxkeypad_activate_all(membase);

	input_sync(&imxkeypad->input);

	/* if any keys are pressed, enable the timer */
	if (num_pressed)
		mod_timer(&imxkeypad->timer, jiffies + SCAN_INTERVAL);
	printk(KERN_ERR "imxkeypad: polling\n");

	if ((!num_pressed)&&(imxkeypad->irq_disabled)) {
		for (row = 0; row < num_irq; row++) {
			enable_irq(irq[row]);
		}
		imxkeypad->irq_disabled = 0;
	}

	spin_unlock_irqrestore(&imxkeypad->lock, flags);


}

/* 
 * imxkeypad interrupt handler.
 */
static irqreturn_t imxkeypad_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct imxkeypad *imxkeypad = dev_id;
	/** wait chattering delay **/
	/* udelay(100); */

	printk(KERN_ERR "imxkeypad: interrupt\n");
	//imxkeypad_scankeyboard(imxkeypad, regs);
	mod_timer(&imxkeypad->timer, jiffies + SCAN_INTERVAL);

	return IRQ_HANDLED;
}

/*
 * LoCoMo timer checking for released keys
 */
static void imxkeypad_timer_callback(unsigned long data)
{
	struct imxkeypad *imxkeypad = (struct imxkeypad *) data;
	imxkeypad_scankeyboard(imxkeypad, NULL);
}



static struct imxkeypad imxkeypad = {
	.phys = "imxkeypad/input0",
};


static int __init imxkeypad_init(void)
{
	int i, ret;

	printk("Initializing Armadeus keypad driver\n");

//	imxkeypad = kmalloc(sizeof(struct imxkeypad), GFP_KERNEL);
//	if (!imxkeypad)
//		return -ENOMEM;

	memset(&imxkeypad, 0, sizeof(struct imxkeypad));

	/* try and claim memory region */
//	if (!request_mem_region((unsigned long) dev->mapbase, 
//				dev->length, 
//				LOCOMO_DRIVER_NAME(dev))) {
//		ret = -EBUSY;
//		printk(KERN_ERR "imxkeypad: Can't acquire access to io memory for keyboard\n");
//		goto free;
//	}

//	imxkeypad->ldev = dev;
//	locomo_set_drvdata(dev, imxkeypad);

//	imxkeypad->base = (unsigned long) dev->mapbase;

	spin_lock_init(&imxkeypad.lock);

	init_timer(&imxkeypad.timer);
	imxkeypad.timer.function = imxkeypad_timer_callback;
	imxkeypad.timer.data = (unsigned long) &imxkeypad;

	imxkeypad.input.evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
	
	init_input_dev(&imxkeypad.input);
	imxkeypad.input.keycode = imxkeypad.keycode;
	imxkeypad.input.keycodesize = sizeof(unsigned char);
	imxkeypad.input.keycodemax = ARRAY_SIZE(imxkeypad_keycode);
	imxkeypad.input.private = &imxkeypad;

	memcpy(imxkeypad.keycode, imxkeypad_keycode, sizeof(imxkeypad.keycode));
	for (i = 0; i < IMXKEYPAD_NUMKEYS; i++)
		set_bit(imxkeypad.keycode[i], imxkeypad.input.keybit);
	clear_bit(0, imxkeypad.input.keybit);

	imxkeypad.input.name = "imxkeypad";
	imxkeypad.input.phys = imxkeypad.phys;
	imxkeypad.input.id.bustype = BUS_HOST;
	//imxkeypad.input.id.vendor = 0x0001;
	//imxkeypad.input.id.product = 0x0001;
	//imxkeypad.input.id.version = 0x0100;


	/* attempt to get the interrupts */
	for (i = 0; i < num_irq; i++) {
		//imxkeypad.irq[i] = IRQ_GPIOA(i+KPD_ROWS_BIT_OFFSET);
		printk(KERN_ERR "imxkeypad: requesting irq %d\n", irq[i]);
		ret = request_irq(irq[i], imxkeypad_interrupt, 0, "imxkeypad", &imxkeypad);
		if (ret) {
			printk(KERN_ERR "imxkeypad: Can't get irq for keyboard\n");
			goto out;
		}
	}
	
	printk(KERN_ERR "imxkeypad io_addr ctrl: %lx, status: %lx\n", io_addr[0], io_addr[1]);

	printk(KERN_ERR "imxkeypad matrix size: %ld rows, %ld cols\n", size[0], size[1]);
	printk(KERN_ERR "imxkeypad bit_offset : rows %ld bits, cols %ld bits\n", bit_offset[0], bit_offset[1]);

	input_register_device(&imxkeypad.input);

	imxkeypad_activate_all(0);

	//mod_timer(&imxkeypad.timer, jiffies + SCAN_INTERVAL);

	printk(KERN_INFO "input: Armadeus keypad on imxkeypad\n");

	return 0;

out:
//	release_mem_region((unsigned long) dev->mapbase, dev->length);
//free:
//	kfree(imxkeypad);



	return 0;

}

static void __exit imxkeypad_exit(void)
{
	int i;
//	driver_unregister(&keyboard_driver.drv);
	
	for (i = 0; i < num_irq; i++) {
		free_irq(irq[i], &imxkeypad);
	}

	del_timer_sync(&imxkeypad.timer);
	
	input_unregister_device(&imxkeypad.input);

//	release_mem_region((unsigned long) dev->mapbase, dev->length);

//	kfree(imxkeypad);

	printk(KERN_INFO "input: Armadeus keypad terminated\n");


}

module_init(imxkeypad_init);
module_exit(imxkeypad_exit);
