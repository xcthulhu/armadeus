/*
 * imxkeypad driver for Linux/ARM Armadeus project
 *
 *  Copyright (c) 2006-2008 Eric Jarrige
 *
 * Based on LoCoMo keyboard driver
 */

/*
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

#include <linux/autoconf.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/ioport.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <linux/version.h>
#include <mach/hardware.h>

MODULE_AUTHOR("Eric Jarrige");
MODULE_DESCRIPTION("i.MXL keypad driver");
MODULE_LICENSE("GPL");

#define DEBUG

#define KPD_MAX_ROWS		8
#define KPD_MAX_COLS		8

#define KB_ROWMASK(r)		(1 << (r + kpdPtr->rows_offset))
#define SCANCODE(c,r)		( (c * kpdPtr->num_rows) + (r) )

#define KB_DELAY		8	/* uS */
#define SCAN_INTERVAL		(HZ/10)

static unsigned short imxkeypad_keycode[KPD_MAX_ROWS * KPD_MAX_COLS] = {
	KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_ESC,       /* 0 - 3 */
	KEY_3,    KEY_6,     KEY_9,     KEY_SPACE,     /* 4 - 7 */
	KEY_2,    KEY_5,     KEY_8,     KEY_0,         /* 8 - 11 */
	KEY_1,    KEY_4,     KEY_7,     KEY_KPASTERISK /* 12 - 15 */
};

struct imxkeypad {
	unsigned short keycode[KPD_MAX_ROWS * KPD_MAX_COLS];
	struct input_dev *input;
	char phys[32];

	int num_rows;
	int rows_offset;
	int num_cols;
	int cols_offset;
	int port;
	int irqs[KPD_MAX_ROWS];
	spinlock_t lock;
	
	struct timer_list timer;
	char	irq_disabled;
	int 	debouncing;
};

/* module parameters definition */
static int io_port=0;
module_param(io_port, int, 0000);
MODULE_PARM_DESC(io_port, "GPIO port (0..3)");

static long int size[]={4,4};
module_param_array(size, long, NULL, 0000);
MODULE_PARM_DESC(size, "matrix size: num rows, num cols");

static long int bit_offset[]={3,7};
module_param_array(bit_offset, long, NULL, 0000);
MODULE_PARM_DESC(bit_offset, "rows and cols bits offset in registers");

module_param_array(imxkeypad_keycode, short, NULL, 0000); // imxkeypad_keycode declared line 60
MODULE_PARM_DESC(imxkeypad_keycode, "rows and cols bits offset in registers");

/* helper functions for reading the keyboard matrix */
static inline void imxkeypad_charge_all(struct imxkeypad *kpdPtr)
{
	DR(kpdPtr->port) |= 
		(((1<<(kpdPtr->num_cols))-1)<<(kpdPtr->cols_offset));
}

static inline void imxkeypad_activate_all(struct imxkeypad *kpdPtr)
{
	DR(kpdPtr->port) &= ~(((1<<(kpdPtr->num_cols))-1)<<(kpdPtr->cols_offset));
}

static inline void imxkeypad_activate_col(struct imxkeypad *kpdPtr, int col)
{
	DR(kpdPtr->port) &= ~(1 << (col + kpdPtr->cols_offset));
}

static inline void imxkeypad_reset_col(struct imxkeypad *kpdPtr, int col)
{
	DR(kpdPtr->port) |= (1 << (col + kpdPtr->cols_offset));
}

/*
 * The i.MX keypad only generates interrupts when a key is pressed.
 * So when a key is pressed, we enable a timer.  This timer scans the
 * keyboard, and this is how we detect when the key is released.
 */

/* Scan the hardware keyboard and push any changes up to the Input layer */
static void imxkeypad_scankeyboard(struct imxkeypad *kpdPtr) 
{
	unsigned int row, col, scancode;
	unsigned long rowd;
	unsigned int num_pressed;
	
	imxkeypad_charge_all(kpdPtr);

	num_pressed = 0;
	for (col = 0; col < kpdPtr->num_cols; col++) {

		imxkeypad_activate_col(kpdPtr, col);
		udelay(KB_DELAY);
		 
		rowd = ~(SSR(kpdPtr->port));
		for (row = 0; row < kpdPtr->num_rows; row++ ) {
			scancode = SCANCODE(col, row);
			if (rowd & KB_ROWMASK(row)) {
				num_pressed++;
				input_report_key(kpdPtr->input, kpdPtr->keycode[scancode], 1);
				kpdPtr->debouncing = 1;
				printk(KERN_DEBUG "key index: %d\n", scancode);
			} else if (1 >= kpdPtr->debouncing) {
				input_report_key(kpdPtr->input, kpdPtr->keycode[scancode], 0);
			}
		}
		imxkeypad_reset_col(kpdPtr, col);
	}
	imxkeypad_activate_all(kpdPtr);

	input_sync(kpdPtr->input);

	/* on key release, repeat some scan to remove debounce */
	if ((!num_pressed)&&(0 <= kpdPtr->debouncing)) {
		kpdPtr->debouncing--;
	}

	if ((!num_pressed)&&(kpdPtr->irq_disabled)&&(0 >= kpdPtr->debouncing)) {
		for (row = 0; row < kpdPtr->num_rows; row++) {
			enable_irq(kpdPtr->irqs[row]);
		}
		kpdPtr->irq_disabled = 0;
	} else {
		/* if any keys are pressed or still checking debounce, enable the timer */
		mod_timer(&kpdPtr->timer, jiffies + SCAN_INTERVAL);
	}
}

/* 
 * imxkeypad interrupt handler.
 */
static irqreturn_t imxkeypad_interrupt(int irq, void *dev_id/*, struct pt_regs *regs*/)
{
	struct imxkeypad *kpdPtr = dev_id;
	unsigned int row;
	 
	if (!kpdPtr->irq_disabled) {
		for (row = 0; row < kpdPtr->num_rows; row++) {
			disable_irq(kpdPtr->irqs[row]);
		}
		kpdPtr->irq_disabled = 1;
		kpdPtr->debouncing = 1;
	}

	printk(KERN_DEBUG "keypad int\n");

	mod_timer(&kpdPtr->timer, jiffies + SCAN_INTERVAL);

	return IRQ_HANDLED;
}

/*
 * imxkeypad timer checking for released keys
 */
static void imxkeypad_timer_callback(unsigned long data)
{
	struct imxkeypad *kpdPtr = (struct imxkeypad *) data;
	imxkeypad_scankeyboard(kpdPtr);
}



static struct imxkeypad imxkeypad;


static int __init imxkeypad_init(void)
{
	int i, ret;

	printk(KERN_INFO "Initializing Armadeus keypad driver\n");

	memset(&imxkeypad, 0, sizeof(struct imxkeypad));

	spin_lock_init(&imxkeypad.lock);

	init_timer(&imxkeypad.timer);
	imxkeypad.timer.function = imxkeypad_timer_callback;
	imxkeypad.timer.data = (unsigned long) &imxkeypad;
	strcpy(imxkeypad.phys, "imxkeypad/input0");
	
	imxkeypad.input = input_allocate_device();
	if (!imxkeypad.input) {
		printk(KERN_ERR "imxkeypad: Can't allocate device structure\n");
		goto out;
	}
	imxkeypad.input->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
	
	imxkeypad.input->keycode = imxkeypad.keycode;
	imxkeypad.input->keycodesize = sizeof(unsigned short);
	imxkeypad.input->keycodemax = ARRAY_SIZE(imxkeypad_keycode);
	imxkeypad.input->name = "imxkeypad";
	imxkeypad.input->phys = imxkeypad.phys;
	imxkeypad.input->id.bustype = BUS_HOST;
	imxkeypad.input->id.vendor = 0x0001;
	imxkeypad.input->id.product = 0x0001;
	imxkeypad.input->id.version = 0x0100;

	printk(KERN_DEBUG "imxkeypad matrix size: %ld rows, %ld cols\n", size[0], size[1]);
	imxkeypad.num_rows = size[0];
	imxkeypad.num_cols = size[1];

	printk(KERN_DEBUG "imxkeypad bit_offset : rows %ld bits, cols %ld bits\n", bit_offset[0], bit_offset[1]);
	imxkeypad.rows_offset = bit_offset[0];
	imxkeypad.cols_offset = bit_offset[1];

	printk(KERN_DEBUG "imxkeypad GPIO port: %d\n", io_port);
	imxkeypad.port = io_port;

	/* init keycode table */	
	memcpy(imxkeypad.keycode, imxkeypad_keycode, sizeof(imxkeypad.keycode));
	for (i = 0; i < (imxkeypad.num_cols*imxkeypad.num_rows); i++)
		set_bit(imxkeypad.keycode[i], imxkeypad.input->keybit);
	clear_bit(0, imxkeypad.input->keybit);

	/* init each gpios row pin as an input with pullup */	
	for (i = 0; i < imxkeypad.num_rows; i++) {
		imx_gpio_mode( (imxkeypad.port<<GPIO_PORT_SHIFT) 
				| GPIO_GIUS | GPIO_IN | GPIO_PUEN
				| (imxkeypad.rows_offset + i) );
	}	
	/* init each gpios column pin as an output */	
	for (i = 0; i < imxkeypad.num_cols; i++) {
		imx_gpio_mode( (imxkeypad.port<<GPIO_PORT_SHIFT) 
				| GPIO_GIUS | GPIO_OUT | GPIO_DR
				| (imxkeypad.cols_offset + i) );
		
	}	

	/* Activate(??) every column to detect any key pressed */
	imxkeypad_activate_all(&imxkeypad);

	/* register driver to the Linux Input layer */
	input_register_device(imxkeypad.input); /* TODO: Handle returned value !! */

	/* attempt to reserve the interrupts (on i.MXL each GPIO can generate it's own interrupt) */
	for (i = 0; i < imxkeypad.num_rows; i++) {
		imxkeypad.irqs[i] = IMX_IRQS + 32*imxkeypad.port + imxkeypad.rows_offset + i;
		printk(KERN_DEBUG "imxkeypad: requesting irq %d\n", imxkeypad.irqs[i]);
		ret = request_irq(imxkeypad.irqs[i], imxkeypad_interrupt, 0, "imxkeypad", &imxkeypad);
		if (ret) {
			printk(KERN_ERR "imxkeypad: Can't get irq for keyboard\n");
			goto free;
		}
		else set_irq_type( imxkeypad.irqs[i], IRQF_TRIGGER_FALLING ); /* configure it as falling edge sensitive */
	}
	
	return 0;

free:
	input_unregister_device(imxkeypad.input);
out:

	return 0;
}

static void __exit imxkeypad_exit(void)
{
	int i;
	
	for (i = 0; i < imxkeypad.num_rows; i++) {
		free_irq(imxkeypad.irqs[i], &imxkeypad);
	}

	del_timer_sync(&imxkeypad.timer);
	
	input_unregister_device(imxkeypad.input);
}

module_init(imxkeypad_init);
module_exit(imxkeypad_exit);
