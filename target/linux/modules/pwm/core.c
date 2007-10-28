/*
 *
 * Driver for the i.MXl PWM driver
 *
 * This driver is part of the Armadeus project and is the merge of a lot of ideas
 * found on the Web.
 *
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 */
//#define DEBUG 1

#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/uaccess.h>        /* get_user,copy_to_user */
#include <linux/miscdevice.h>

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/pm.h>
// For /sys
#include <linux/sysdev.h>
// For struct class
#include <linux/device.h>
// For circular buffer
#include <linux/circ_buf.h> 

#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>
// For register adresses
#include <asm/arch/imx-regs.h>
// additional defines
#define PWMC_PRESCALER_MASK 0x00007F00
#define PWMC_REPEAT_MASK    0x0000000C
#define PWMC_CLKSEL_MASK    0x00000003
//
#include <linux/platform_device.h>

#include "pwm.h"

#define PWM_TEST

#define DRIVER_NAME         "imx-pwm"
#define DRIVER_VERSION      "0.5"

#define DEV_IRQ_NAME    DRIVER_NAME
#define DEV_IRQ_ID      DRIVER_NAME

//#define	PK(fmt, args...)	printk(fmt, ## args)
#define	PK(...)


struct pwm_device {
    u32 reg;
    int active;
    unsigned duty;
    /*const*/ struct pwm_table *entry;
    struct class_device class_dev;
};

static struct class pwm_class;	/* forward declaration only */

#define to_pwm_device(d) container_of(d, struct pwm_device, class_dev)


// Global variables
int     gPwmMode;
//char    *gpWriteBuf;
int     g4bytes; // count 4 bytes write occurence to FIFO
u8      *gpWriteBufPtr;
u16     *gpWriteBufPtr16;
int     gDataLen = PWM_DATA_8BIT;
u16     gSampleValue;

int     irq = 0;
module_param(irq, int, 0);

wait_queue_head_t write_wait;
wait_queue_head_t exit_wait;
struct fasync_struct *ts_fasync;

int gMajor = 0;                 /* TODO dynamic major for now */

typedef struct timer_list timer_blk_t;

typedef struct timerStruct
{
    timer_blk_t	*timer_blk_ptr;
    int     period;
    void    (*timer_func)(unsigned long);
    int     stop_flag;
} PWM_Timer_t;

static PWM_Timer_t pwmTimer;
static timer_blk_t timer_blk;

struct pwm_table {
    long period;	/* in us */
    unsigned max_period_val;
    unsigned prescaler;
};

struct pwm_table pwmdata;
/*
 * Ready to use calculations
 */
static const struct pwm_table pwm_table[]={
    {
        .period=1,	/* 1 us */
        .max_period_val = 13,
        .prescaler=1,
    },
    {
        .period=5,	/* 5 us */
        .max_period_val = 65,
        .prescaler=1,
    },
    {
        .period=10,	/* 10 us */
        .max_period_val = 130,
        .prescaler=1,
    },
    {
        .period=20,	/* 20 us */
        .max_period_val = 260,
        .prescaler=1,
    },
    {
        .period=50,	/* 50 us */
        .max_period_val = 650,
        .prescaler=1,
    },
    {
        .period=100,	/* 100 us */
        .max_period_val = 650,
        .prescaler=2,
    },
    {
        .period=500,	/* 500 us */
        .max_period_val = 929,
        .prescaler=7,
    },
    {
        .period=1000,	/* 1ms */
        .max_period_val = 1000,
        .prescaler=13,
    },
    {
        .period=2000,	/* 2ms */
        .max_period_val = 1000,
        .prescaler=26,
    },
    {
        .period=3000,	/* 3ms */
        .max_period_val = 1000,
        .prescaler=39,
    },
    {
        .period=4000,	/* 4ms */
        .max_period_val = 1020,
        .prescaler=51,
    },
    {
        .period=5000,	/* 5ms */
        .max_period_val = 1016,
        .prescaler=64,
    }
};

/****************************************************************
 * Circular buffer handling four sound playing
 ***************************************************************/

struct sound_circ_buf {
    char* buf;
    int write; // == head
    int read;  // == tail
    int size;
};

static inline u8 get_byte_from_circbuf( struct sound_circ_buf* abuffer )
{
    u8 b = abuffer->buf[ abuffer->read ];
    abuffer->read = (abuffer->read + 1) & (abuffer->size - 1);
    return b;
}

static inline u16 get_word_from_circbuf( struct sound_circ_buf* abuffer )
{
    u16 w = *((u16*)((abuffer->buf)[ abuffer->read ]));
    abuffer->read = (abuffer->read + 2) & (abuffer->size - 1);
    return w;
}

static inline void put_byte_to_circbuf( struct sound_circ_buf* abuffer, unsigned char abyte )
{
    abuffer->buf[ abuffer->write ] = abyte;
    abuffer->write = (abuffer->write + 1) & (abuffer->size - 1);
}

static inline void increase_circbuf( struct sound_circ_buf* abuffer, int count )
{
    abuffer->write = (abuffer->write + count) & (abuffer->size - 1);
    pr_debug("Added %d bytes to buffer, now write is in pos %d\n", count, abuffer->write);
}

#define MAX_SOUND_BUFFER_SIZE (16*1024) // !! Should be a power of 2 !!
static struct sound_circ_buf gPWM_Circ_Buf;

/****************************************************************
 *
 ***************************************************************/

static void inline unregister_sys_file( struct pwm_device *ppd );


/**
 * setup_pwm_unit - common setup function whenever something was changed
 * @ppd: PWM unit to work with
 */
static void setup_pwm_unit(struct pwm_device *ppd)
{
    if (ppd->entry == NULL)
            return;	/* do nothing, not fully configured yet */

    if (ppd->active) 
    {
        // Activate PWM
        PWMC |= PWMC_EN;

        // Setup prescaler
        pr_debug("Updating PWM registers\n");
        PWMC = (PWMC & ~PWMC_PRESCALER_MASK) | PWMC_PRESCALER(ppd->entry->prescaler) /*| PWMC_REPEAT(3)*/;

        // Setup period
        PWMP = PWMP_PERIOD(ppd->entry->period);

        // Setup duty
        PWMS = PWMS_SAMPLE( (u32) ((ppd->entry->period * ppd->duty) / 1000) );
/*        unsigned duty;

        if (ppd->duty == MAX_DUTY)
                duty=ppd->entry->max_period_val-1;	// FIXME: Solve with bit 10 in this register
        else
                duty=((ppd->entry->max_period_val-1)*ppd->duty)/MAX_DUTY;
*/

    }
    else 
    {
        // De-activate
        PWMC &= ~PWMC_EN;
    }
}

/******************************************************************************
 * Function Name: checkDevice
 *
 * Input: 		inode	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: verify if the inode is a correct inode
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
// static int checkDevice(struct inode *pInode)
// {
// 	int minor;
// 	kdev_t dev = pInode->i_rdev;
// 
// 	if( MAJOR(dev) != gMajor)
// 	{
// 		PK("<1>checkDevice bad major = %d\n",MAJOR(dev) );
// 		return -1;
// 	}
// 	minor = MINOR(dev);
// 
// 	if ( minor < MAX_ID )
// 		return minor;
// 	else
// 	{
// 		PK("<1>checkDevice bad minor = %d\n",minor );
// 		return -1;
// 	}
// }

/* timer */
int CreateTimer(PWM_Timer_t *timer)
{
    init_timer(timer->timer_blk_ptr);
    timer->timer_blk_ptr->function = timer->timer_func;

    return(0);
}

int StartTimer(PWM_Timer_t *timer)
{
    timer->timer_blk_ptr->expires = jiffies + timer->period;
    timer->stop_flag = 0;

    add_timer((struct timer_list *)timer->timer_blk_ptr);

    return(0);
}
int StopTimer(PWM_Timer_t *timer)
{
    timer->stop_flag = 1;
    del_timer_sync((struct timer_list *)timer->timer_blk_ptr);

    return(0);
}

static void StopPwm(void)
{
    PWMC &= ~PWMC_IRQEN;    // disable IRQ
    PWMC &= ~PWMC_EN;       // disable PWM

    gSampleValue = 0;
    if(gPwmMode == PWM_TONE_MODE)
        StopTimer(&pwmTimer);

    // Can release file now
    wake_up_interruptible(&exit_wait);

    PK("<1>data completed.\n");
    PK("<1>PWMC = 0x%8x\n", (u32)PWMC);
}

static void pwmIntFunc( unsigned long unused)
{
//    u32	period;

    PK("<1>pwm sam int\n");

//     if (gWriteCnt > 0)
//     {
//         PK("<1>PWMS = 0x%4x\n", (u16)_reg_PWM_PWMS);
// 
//         // 96M/128/2/period
//         period = *gpWriteBufPtr16++;
// 
//         if(period > 0)
// //			PWMP = 96000000/128/12/2/period; //96000000/128/2*11
//             PWMP = 66000000/128/2/period; //96000000/128/2*11
//         PK("<1>PWMP = 0x%4x\n", (u16)PWMP);
//         PK("<1>PWMCNT = 0x%4x\n", (u16)PWMCNT);
// 
//         StartTimer(&pwmTimer);
// 
//         gWriteCnt--;
//     }
//     else
//         StopPwm();

}

// static int handle_pm_event(struct pm_dev *dev, pm_request_t rqst, void *data)
// {
//     switch(rqst)
//     {
//         case PM_SUSPEND:
//             PK("<1> PM suspend event received in PWM device.\n");
//             StopPwm();
//         break;
// 
//         case PM_RESUME:
//             PK("<1> PM resume event received in PWM device.\n");
//         break;
// 
//         default:
//         break;
//     }
//     return 0;
// }

/******************************************************************************
 *
 *****************************************************************************/
static int initPWM(void)
{
    // Init Port PA[2] : PWMO
    // Should be done in apf9328.c non ???
    DDIR(0) |=  0x00000002;
    GIUS(0) &= ~0x00000002;
    GPR(0)  &= ~0x00000002;

    // Software reset
    PWMC |= PWMC_SWR;
    udelay(10);
    // Activate & de-activate PWM (seems to be necessary after a reset)
    PWMC |= PWMC_EN; PWMC &= ~PWMC_EN; 

    // Enable interrupt
    //PWMC |= PWMC_IRQEN;

    return 1;
}

//////////////////////////////////////// /DEV INTERFACE ////////////////////////////////////////////

/*
 * Called from user space close()
 */
int pwm_release(struct inode * inode, struct file * filp)
{
    // wait unit gWriteCnt == 0
    interruptible_sleep_on(&exit_wait);

    if( gPWM_Circ_Buf.buf ) {
        kfree( gPWM_Circ_Buf.buf );
        gPWM_Circ_Buf.buf = 0;
    }
    printk(DRIVER_NAME " released, 4 bytes FIFO usage: %d\n", g4bytes);
    return 0;
}

/*
 * Called from user space open()
 */
int pwm_open(struct inode * inode, struct file * filp)
{
    // Init PWM hardware
    initPWM();

    pr_debug(DRIVER_NAME " opened \n");
    return 0;
}

/*
 *
 */
static int pwm_fasync(int fd, struct file *filp, int mode)
{
    PK("<1>in pwm_fasyn ----\n");
#if 0
    /* TODO TODO put this data into file private data */
    int minor = checkDevice( filp->f_dentry->d_inode);
    if ( minor == - 1)
    {
        PK("<1>asp_fasyn:bad device minor\n");
        return -ENODEV;
    }

    return( fasync_helper(fd, filp, mode, &ts_fasync) );
#endif
    return 0;
}

/*
 * Called from user space ioctl()
 */
int pwm_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    char *str=NULL;
    int ret = 0;

    pr_debug("pwm_ioctl ----\n");
    switch(cmd)
    {
        // Set PWM Mode (Tone or Playback)
        case PWM_IOC_SMODE:
        {
            gPwmMode = arg;

            if (gPwmMode == PWM_TONE_MODE)
            {
                // create periodic timer when tone mode
                pr_debug("PWM Tone Mode.\n");
                pwmTimer.timer_blk_ptr = &timer_blk;
                pwmTimer.timer_func = pwmIntFunc;
                CreateTimer(&pwmTimer);
            }
            else
            {
                pr_debug("PWM Play Mode.\n");
            }
        }
        break;

        // Set Playback frequency/ouput rate
        case PWM_IOC_SFREQ:
        {
            PWMC &= ~PWMC_EN;                       // Disable PWM
            PWMC &= ~PWMC_CLKSRC;                   // Select 16MHz PERCLK1 as CLK source
            PWMC &= ~PWMC_PRESCALER_MASK;           // PRESCALER = 0   =>  use full 16MHz

            switch(arg)
            {
                case PWM_SAMPLING_32KHZ:
                    PWMC &= ~PWMC_REPEAT_MASK;      // REPEAT = 0 => divide by 1
                    PWMC &= ~PWMC_CLKSEL_MASK;      // CLKSEL = 0 => divide by 2
                    // 16MHz/256/1/2 = 32768
                    str = "32Khz";
                    break;
                case PWM_SAMPLING_16KHZ:
                    PWMC &= ~PWMC_REPEAT_MASK;
                    PWMC |= PWMC_REPEAT(1);         // REPEAT = 01 => divide by 2
                    PWMC &= ~PWMC_CLKSEL_MASK;      // CLKSEL = 0 => divide by 2
                    // 16MHz/256/2/2 = 16384
                    str = "16Khz";
                    break;
                case PWM_SAMPLING_8KHZ:
                default:
                    PWMC &= ~PWMC_REPEAT_MASK;
                    PWMC |= PWMC_REPEAT(2);         // REPEAT = 10 => divide by 4
                    PWMC &= ~PWMC_CLKSEL_MASK;      // CLKSEL = 0 => divide by 2
                    // 16MHz/256/4/2 = 8192
                    str = "8Khz";
                    break;
            }
            pr_debug("Sample rate = %s\n", str);
            pr_debug("ioctl: PWMC = 0x%8x\n", PWMC);
        }
        break;

        // Set samples length (8 or 16 bits)
        case PWM_IOC_SDATALEN:
        {
            gDataLen = arg;
            if( arg == PWM_DATA_8BIT )
            {
                PWMP = 0xfe; // Period, 8bit
                str = "8bit";
            }
            else // if(arg == PWM_DATA_16BIT)
            {
                PWMP = 0xfffe; // Period, 16bit
                str = "16bit";
            }

            pr_debug("Data Length = %s\n", str);
        }
        break;

        case PWM_IOC_SSAMPLE:
            gSampleValue = arg;
        break;

        case PWM_IOC_SPERIOD:
            if (gPwmMode == PWM_TONE_MODE)
            {
                PK("<1>PWM period = %d\n",(int)arg);
                pwmTimer.period = arg/12;
            }
        break;

        case PWM_IOC_STOP:
            StopPwm();
        break;

        case PWM_IOC_SWAPDATA:
            if (arg&PWM_SWAP_HCTRL) //Halfword FIFO data swapping 
            {
                PWMC |= PWMC_HCTR;
            }
            else
            {
                PWMC &= ~PWMC_HCTR;
            }

            if (arg&PWM_SWAP_BCTRL) //Byte FIFO data swapping 
            {
                PWMC |= PWMC_BCTR;
            }
            else
            {
                PWMC &= ~PWMC_BCTR;
            }
        break;

        default:
            printk( DRIVER_NAME ": unkown IOCTL\n");
        break;
    }

    return ret;
}

/******************************************************************************
 * Called from user space read()
 *
 *****************************************************************************/
ssize_t pwm_read(struct file * filp, char * buf, size_t count, loff_t * l)
{
    int ret = 0;

    if( PWMC & PWMC_IRQEN )
        ret = 1; // no data
    else
        ret = 0; // processing data

    pr_debug("pwm_read ----\n");
    return ret;
}

/******************************************************************************
 * Called from user space write()
 *
 *****************************************************************************/
static ssize_t pwm_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos )
{
    int ret = 0;
    int remaining_space = 0;

    pr_debug( "pwm_write ----\n" );

    if( count > MAX_SOUND_BUFFER_SIZE )
        count = MAX_SOUND_BUFFER_SIZE;
    // First time, allocate the double buffer
    if( !gPWM_Circ_Buf.buf ) {
        gPWM_Circ_Buf.buf = kmalloc( count*2, GFP_KERNEL );
        if( !gPWM_Circ_Buf.buf ) {
            goto out;
        } else {
            // Initialize circular buffer
            gPWM_Circ_Buf.read  = 0;
            gPWM_Circ_Buf.write = 0;
            gPWM_Circ_Buf.size  = MAX_SOUND_BUFFER_SIZE*2;
        }
    }

    // If there is enough space at the end of the buffer for all data in one copy, do it
    remaining_space = gPWM_Circ_Buf.size - gPWM_Circ_Buf.write;
    pr_debug("Remaining space to end: %d, must write: %d \n", remaining_space, count);
    if( remaining_space >= count ) {
        // Get data from UserSpace
        if( copy_from_user( gPWM_Circ_Buf.buf + gPWM_Circ_Buf.write , buf, count ) ) {
            ret = -EFAULT; goto out;
        }
        increase_circbuf( &gPWM_Circ_Buf, count );
    // if not enough space, then
    } else {
        // Do it in 2 times
        if( copy_from_user( gPWM_Circ_Buf.buf + gPWM_Circ_Buf.write, buf, remaining_space ) ) {
            ret = -EFAULT; goto out;
        }
        increase_circbuf( &gPWM_Circ_Buf, remaining_space );
        if( copy_from_user( gPWM_Circ_Buf.buf + gPWM_Circ_Buf.write, buf, count - remaining_space ) ) {
            ret = -EFAULT; goto out;
        }
        increase_circbuf( &gPWM_Circ_Buf, count - remaining_space );
    }

    // Enable PWM
    PWMC |= PWMC_EN; PWMS = 0x00000000;

    if( gPwmMode == PWM_PLAY_MODE )
    {
        PWMC |= PWMC_IRQEN; // Enable IRQ
    }
    else
    {
        PWMS = gSampleValue;
        PWMC |= 0x7f00;	// set prescaler to 128
/*        gpWriteBufPtr16 = (u16*)gpWriteBuf;
        gWriteCnt /= 2; //input size is 8bit size, need /2 to get 16bit size
        StartTimer( &pwmTimer );*/
    }

    // Calculate how much space is left in buffer
    remaining_space = CIRC_SPACE( gPWM_Circ_Buf.write, gPWM_Circ_Buf.read, gPWM_Circ_Buf.size );
    pr_debug("Space left: %d\n", remaining_space);
    if( remaining_space < count ) {
        // Pauses user app until interrupt handler has consumed some data
        interruptible_sleep_on( &write_wait );
    }

out:
    pr_debug( "out pwm_write PWMC = 0x%8x ----\n", PWMC );
    return count;
}


struct file_operations pwm_fops = {
    open:           pwm_open,
    release:        pwm_release,
    read:           pwm_read,
    write:          pwm_write,
    ioctl:          pwm_ioctl,
    fasync:         pwm_fasync
};

/////////////////////////////////////// END OF /DEV INTERFACE /////////////////////////////////////////////////////////


/**
 * pwm_show_duty - return current duty setting
 * @dev: device to work with
 * @buf: space to write the current value in
 */
static ssize_t pwm_show_duty(struct class_device *dev, char *buf)
{
    ssize_t ret_size = 0;
    struct pwm_device *ppd=to_pwm_device(dev);

    ret_size=sprintf(buf,"%u",ppd->duty);
    return ret_size;
}

/**
 * pwm_store_duty - set new duty
 * @dev: device to work with
 * @buf: buffer with new value in ASCII
 * @size: size of buffer
 *
 * Returns the given @size or -EIO
 *
 * Duty value can be 0 ... 1000. Without floating point you can take
 * one position after decimal point into account. The internal counters
 * supports 10 bit resolution, so it makes sense to support such a thing
 * like 66.6% or 33.3% (given as 666 and 333 values)
 */
static ssize_t pwm_store_duty(struct class_device *dev, const char *buf, size_t size)
{
    long value;
    struct pwm_device *ppd=to_pwm_device(dev);

    value = simple_strtol(buf, NULL, 10);
    if ((value < 1) || (value > 999))
            return -EIO;

    ppd->duty=(unsigned)value;

    setup_pwm_unit(ppd);
    return size;
}

/**
 * pwm_show_period - return current period setting
 * @dev: device to work with
 * @buf: space to write the current value in
 */
static ssize_t pwm_show_period(struct class_device *dev, char *buf)
{
    ssize_t ret_size = 0;
    struct pwm_device *ppd=to_pwm_device(dev);

    if (ppd->entry)	/* already initialized? */
        ret_size=sprintf(buf,"%ld",ppd->entry->period);
    else
        ret_size=sprintf(buf,"0");

    return ret_size;
}

/**
 * pwm_store_period - set new period setting
 * @dev: device to work with
 * @buf: buffer with new value in ASCII
 * @size: size of buffer
 *
 * Returns the given @size or -EIO
 *
 * The min/max period depends on processors features.
 * Currently the i.MXl uses either PERCLK1 or CLK32 clock.s You can prescale
 * these clocks up to 1/128 (down to 203.125kHz).
 * The period is based on this prescaled clock and it's width depends on
 * the period register (PWMP). This register is 16 bits width.
 * So for PERCLK1 (16Mhz):
 * - min. period is (no prescale, PWMP = 1): ~62ns
 * - max. period is (max prescale, period control = 0xFFFF): ~520ms
 * For CLK32 (32KHz):
 * - min. period is ~31us
 * - max. period is 256s 
 *
 * The unit of the given value should be "us" (1E-6) and the value
 * between 1 and 500000.
 */

#define PWM_MAX_PERIOD 500000    // us
#define PWM_MIN_PERIOD 1         //us
#define PWM_MIN_FREQUENCY 2      // Hz
#define PWM_MAX_FREQUENCY 100000 // Hz
static ssize_t pwm_store_period(struct class_device *dev, const char *buf, size_t size)
{
    long value/*,i*/;
    struct pwm_device *ppd=to_pwm_device(dev);

    value = simple_strtol(buf, NULL, 10);
    if ((value < PWM_MIN_PERIOD) && (value > PWM_MAX_PERIOD))
        return -EIO;

/*    for( i=0; i<ARRAY_SIZE(pwm_table); i++ )
    {
        if (pwm_table[i].period >= value)
                break;
    }
    if (i==ARRAY_SIZE(pwm_table))	// period not found in table?
            return -EIO;

    ppd->entry=&pwm_table[i];*/
    ppd->entry=&pwmdata;
    ppd->entry->period = value;
    ppd->entry->prescaler = 8; // 1us / tick if CLKSEL is  = 0
    ppd->entry->max_period_val = value;
    /*printk(KERN_INFO "New period should be %ld us, and is %ld us\n",
            value,ppd->entry->period);*/

    setup_pwm_unit(ppd);
    return size;
}

/**
 * pwm_show_frequency - return current frequency setting
 * @dev: device to work with
 * @buf: space to write the current value in
 */
static ssize_t pwm_show_frequency(struct class_device *dev, char *buf)
{
    ssize_t ret_size = 0;
    struct pwm_device *ppd=to_pwm_device(dev);

    if (ppd->entry)
        ret_size = sprintf( buf,"%ld", (u32) 1000000/(ppd->entry->period) );
    else
        ret_size = sprintf(buf,"0");

    return ret_size;
}

/**
 * pwm_store_frequency - set new frequency setting
 * @dev: device to work with
 * @buf: buffer with new value in ASCII
 * @size: size of buffer
 *
 * Returns the given @size or -EIO
 *
 * The unit of the given value should be in Hertz and the value
 *  between 2 and 100k.
 */

static ssize_t pwm_store_frequency(struct class_device *dev, const char *buf, size_t size)
{
    long value;
    struct pwm_device *ppd=to_pwm_device(dev);

    value = simple_strtol(buf, NULL, 10);
    if( (value < PWM_MIN_FREQUENCY) && (value > PWM_MAX_FREQUENCY) )
        return -EIO;

    // Convert frequency into period
    value = (u32) (1000000 / value);
    //
    ppd->entry=&pwmdata;
    ppd->entry->period = value;
    ppd->entry->prescaler = 8; // 1us / tick if CLKSEL is  = 0
    ppd->entry->max_period_val = value;
    // Update hardware
    setup_pwm_unit(ppd);

    return( size );
}

/**
 * pwm_show_state - return current state setting
 * @dev: device to work with
 * @buf: space to write the current value in
 */
static ssize_t pwm_show_state(struct class_device *dev, char *buf)
{
    ssize_t ret_size = 0;
    struct pwm_device *ppd=to_pwm_device(dev);

    ret_size=sprintf(buf,"%d",ppd->active);
    return ret_size;
}

/**
 * pwm_store_state - set new state setting
 * @dev: device to work with
 * @buf: buffer with new value in ASCII
 * @size: size of buffer
 *
 * Returns the given @size
 */
static ssize_t pwm_store_state(struct class_device *dev, const char *buf, size_t size)
{
    long value;
    struct pwm_device *ppd=to_pwm_device(dev);

    value = simple_strtol(buf, NULL, 10);
    if (value != 0)
        ppd->active=1;
    else
        ppd->active=0;

    setup_pwm_unit(ppd);

    return size;
}


/*
 * Attribute: /sys/class/pwm/pwmX/duty
 */
static struct class_device_attribute attr_pwm_duty = {
    .attr  = { .name = "duty", .mode = 0644, .owner = THIS_MODULE },
    .show  = pwm_show_duty,
    .store = pwm_store_duty,
};

/*
 * Attribute: /sys/class/pwm/pwmX/period
 */
static struct class_device_attribute attr_pwm_period = {
    .attr  = { .name = "period", .mode = 0644, .owner = THIS_MODULE },
    .show  = pwm_show_period,
    .store = pwm_store_period,
};

/*
 * Attribute: /sys/class/pwm/pwmX/frequency
 */
static struct class_device_attribute attr_pwm_frequency = {
    .attr  = { .name = "frequency", .mode = 0644, .owner = THIS_MODULE },
    .show  = pwm_show_frequency,
    .store = pwm_store_frequency,
};

/*
 * Attribute: /sys/class/pwm/pwmX/active
 */
static struct class_device_attribute attr_pwm_active = {
    .attr  = { .name = "active", .mode = 0644, .owner = THIS_MODULE },
    .show  = pwm_show_state,
    .store = pwm_store_state,
};

/**
 * init_userinterface - setup the device and things the user will see
 * @pdev: platform device to work with
 * @address: physical address of this device
 *
 * Return 0 on success
 */
static int init_userinterface(struct platform_device *pdev, u32 address)
{
    int rc = 0;
    struct pwm_device *ppd;

    if (unlikely((ppd = kmalloc(sizeof(struct pwm_device), GFP_KERNEL)) == NULL)) {
		rc = -ENOMEM;
        goto error_malloc;
	}

    memset(&ppd->class_dev, 0, sizeof(struct class_device));
    ppd->class_dev.class = &pwm_class;
    ppd->class_dev.dev=&pdev->dev;
    snprintf(ppd->class_dev.class_id, BUS_ID_SIZE, "pwm%i", pdev->id);
    ppd->reg=address;
    ppd->entry=NULL;
    ppd->active=0;
    ppd->duty=500; // = 50.O%

    rc = class_device_register(&ppd->class_dev);
    if (unlikely(rc)) goto error_register;

    // Register the attributes
    rc |= class_device_create_file( &ppd->class_dev, &attr_pwm_duty );
    if (unlikely(rc)) goto error_file;
    rc |= class_device_create_file( &ppd->class_dev, &attr_pwm_period );
    if (unlikely(rc)) goto error_file;
    rc |= class_device_create_file( &ppd->class_dev, &attr_pwm_frequency );
    if (unlikely(rc)) goto error_file;
    rc |= class_device_create_file( &ppd->class_dev, &attr_pwm_active );
    if (unlikely(rc)) goto error_file;

    pdev->dev.driver_data=ppd;
	goto success;

error_file:
	printk(KERN_ERR "%s: class /sys file creation failed\n", DRIVER_NAME);
	unregister_sys_file(ppd);
error_register:
	printk(KERN_ERR "%s: class registering failed\n", DRIVER_NAME);
	kfree(ppd);
error_malloc:
	return rc;
success:
    return 0;
}

/*
 *  PWM interrupt handler (used in Play Mode only)
 */
static irqreturn_t pwm_interrupt(int irq, void *dev_id)
{
    u32 status;
    int remaining = 0;

    // Acknowledge interrupt
    status = PWMC;

    remaining = CIRC_CNT( gPWM_Circ_Buf.write, gPWM_Circ_Buf.read, gPWM_Circ_Buf.size );
    if( remaining <= 1024 && remaining > 1021 ) { // TODO make it better (I think a test each consumed sample is the only accurate solution
        // Can accept new data now
        wake_up_interruptible( &write_wait );
    }

    // End of sound buffer
    if( remaining < 3 )
    {
        if(gDataLen == PWM_DATA_8BIT)
        {
            while( remaining )
            {
                PWMS = (u32)(get_byte_from_circbuf( &gPWM_Circ_Buf ));
                /*PK("<1>(%d) pwm_int: PWMS = 0x%8x\n", gWriteCnt, PWMS);*/
                remaining--;
            }
        }
        else // if(gDataLen == PWM_DATA_16BIT)  // one comparaison is enough
        {
            while( remaining )
            {
                PWMS = (u32)(get_word_from_circbuf( &gPWM_Circ_Buf ));
                //PK("<1>(%d) pwm_int: PWMS = 0x%8x\n", gWriteCnt, PWMS);
                remaining--;
            }
        }

        StopPwm();
        goto end;
    }

    // Put sound samples in FIFO
    if(gDataLen == PWM_DATA_8BIT)
    {
        PWMS = (u32)(get_byte_from_circbuf( &gPWM_Circ_Buf ));
        PWMS = (u32)(get_byte_from_circbuf( &gPWM_Circ_Buf ));
        PWMS = (u32)(get_byte_from_circbuf( &gPWM_Circ_Buf ));
        if (PWMC & PWMC_FIFOAV)                 // TODO: verify if needed
        {
            PWMS = (u32)(get_byte_from_circbuf( &gPWM_Circ_Buf ));
            g4bytes++;
        }
    }
    else // if(gDataLen == PWM_DATA_16BIT)  // one comparaison is enough
    {
        // TODO: put good value in PWMC_BCTR to auto swap bytes if needed (do it at write or ioctl)
        PWMS = (u32)(get_word_from_circbuf( &gPWM_Circ_Buf ));
        PWMS = (u32)(get_word_from_circbuf( &gPWM_Circ_Buf ));
        PWMS = (u32)(get_word_from_circbuf( &gPWM_Circ_Buf ));
        if (PWMC & PWMC_FIFOAV)                 // TODO: verify if needed
        {
            PWMS = (u32)(get_word_from_circbuf( &gPWM_Circ_Buf ));
            g4bytes++;
        }
    }

end:
    return(IRQ_HANDLED);
}

/*
 *
 */
static int __init pwm_init_from_probe(void)
{
    int result;

    // Register our char device
    result = register_chrdev(0, DRIVER_NAME, &pwm_fops);
    if ( result < 0 )
    {
        printk("pwm driver: Unable to register driver\n");
        goto end;
    }
    // Dynamic Major allocation
    if( gMajor == 0 )
    {
        gMajor = result;
        printk("PWM major number = %d\n",gMajor);
    }

    result = request_irq(PWM_INT, pwm_interrupt, IRQF_DISABLED, DEV_IRQ_NAME, DEV_IRQ_ID);
    if (result)
    {
        printk("<1>cannot init major= %d irq=%d\n", gMajor, irq);
        unregister_chrdev(gMajor, DRIVER_NAME);
        goto end;
    }

    // init wait queue
    init_waitqueue_head(&write_wait);
    init_waitqueue_head(&exit_wait);
    pr_debug("Wait queues initialized\n");

    // init PWM hardware module
    initPWM();

    printk("PWM driver v" DRIVER_VERSION "\n");

end:
    return( result );
}

/**
 * imx_pwm_drv_probe - claim resources
 * @pdev: platform device to work with
 *
 * CAUTION: This functions rejects a PWM unit if its
 * not forwarded to any possible GPIO pin! In this case
 * it also returns -ENODEV
 *
 * Returns -ENODEV if there is no such device on this SoC, -EBUSY if this
 * resource is already claimed
 */
static int imx_pwm_drv_probe(struct platform_device *pdev)
{
    int err=-ENODEV;
    struct resource *res;

    printk(KERN_INFO " Initializing PWM#%u...",pdev->id);

    switch( pdev->id ) 
    {
        // PWM0
        case 0:
            // Check if GPIO was initialized ??
        break;

        default:
            printk(DRIVER_NAME " failed. Unknown PWM%u device. Remember that this device only supports 1 PWM\n", pdev->id);
            return err;
        break;
    }

    if (unlikely(!(res = platform_get_resource(pdev, IORESOURCE_MEM, 0))))
            goto exit;

    if (unlikely(!request_mem_region( res->start, res->end - res->start + 1, "imx-pwm" ))) {
            err = -EBUSY;
            goto exit;
    }
    pwm_init_from_probe();

    // Create /sys stuf
    if (unlikely((err=init_userinterface(pdev,res->start))))
            goto release;

    printk("done.\n");
    return(0);

release:
    release_mem_region(res->start, res->end - res->start + 1);
exit:
    printk(KERN_INFO "not available.\n");
    return err;
}

static inline void unregister_sys_file( struct pwm_device *ppd )
{
    // Unregister /sys attributes
    class_device_remove_file( &ppd->class_dev, &attr_pwm_active );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_period );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_frequency );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_duty   );
}

/**
 * imx_pwm_drv_remove - free claimed resources
 * @pdev: platform device to work with
 */
static int imx_pwm_drv_remove (struct platform_device *pdev)
{
    struct resource *res;
    struct pwm_device *ppd=(struct pwm_device*)pdev->dev.driver_data;

	unregister_sys_file(ppd);

    class_device_unregister( &ppd->class_dev );

    kfree(ppd);

    res = platform_get_resource( pdev, IORESOURCE_MEM, 0 );
    release_mem_region( res->start, res->end - res->start + 1 );

    return 0;
}

#ifdef CONFIG_PM
/**
 * imx_pwm_drv_suspend - power down
 * @dev: platform device to work with
 * @state: state to reach
 */
static int imx_pwm_drv_suspend (struct platform_device *pdev, pm_message_t state)
{
    printk("%s\n",__FUNCTION__);
    return 0;
}

/**
 * imx_pwm_drv_resume - power up and continue
 * @pdev: platform device to work with
 */
static int imx_pwm_drv_resume (struct platform_device *pdev)
{
    printk("%s\n",__FUNCTION__);
    return 0;
}
#else

# define imx_pwm_drv_suspend NULL
# define imx_pwm_drv_resume NULL

#endif // CONFIG_PM

/**
 * pwm_class_release -
 * @dev:
 */
static void pwm_class_release(struct class_device *dev)
{
    return;
}

static struct platform_driver imx_pwm_driver = {
    .probe      = imx_pwm_drv_probe,
    .remove     = imx_pwm_drv_remove,
    .suspend    = imx_pwm_drv_suspend,
    .resume     = imx_pwm_drv_resume,
    .driver     = {
        .name   = DRIVER_NAME,
    },
};


static struct sysdev_class pwm_sysclass = {
    set_kset_name("pwm"),
};

static struct sys_device pwm_sys_device = {
    .id     = 0,
    .cls    = &pwm_sysclass,
};

static struct class pwm_class = {
    .name       = "pwm",
    .release    = pwm_class_release,
};


/*
 *  Called when module is insmoded
 */
int __init imx_pwm_init(void)
{
    int ret;

    printk(KERN_INFO "Initializing PWM class.\n");

    if (unlikely((ret = class_register(&pwm_class)))) {
        printk(KERN_ERR "%s: couldn't register class, exiting\n", DRIVER_NAME);
        return ret;
    }

    if (unlikely((ret = sysdev_class_register(&pwm_sysclass)))) {
        printk(KERN_ERR "%s: couldn't register sysdev class, exiting\n", DRIVER_NAME);
        goto out_sysdev_class;
    }

    if (unlikely((ret = sysdev_register(&pwm_sys_device)))) {
        printk(KERN_ERR "%s: couldn't register sysdev, exiting\n", DRIVER_NAME);
        goto out_sysdev_register;
    }

    return platform_driver_register( &imx_pwm_driver );

out_sysdev_register:
    sysdev_class_unregister( &pwm_sysclass );
out_sysdev_class:
    class_unregister( &pwm_class );

    return ret;
}

/*
 *  Called when module is rmmoded
 */
void __exit imx_pwm_exit(void)
{
    disable_irq( PWM_INT ); // should be done before free_irq no ?
    free_irq( PWM_INT, DEV_IRQ_ID );
    unregister_chrdev( gMajor, DRIVER_NAME );

    platform_driver_unregister( &imx_pwm_driver );
    sysdev_unregister( &pwm_sys_device );
    sysdev_class_unregister( &pwm_sysclass );
    class_unregister( &pwm_class );

    printk( DRIVER_NAME " successfully unloaded\n" );
}

module_init(imx_pwm_init);
module_exit(imx_pwm_exit);
MODULE_AUTHOR("Julien Boibessot & Sebastien Royen, inspired by a lot of other PWM drivers");
MODULE_DESCRIPTION("Armadeus PWM driver");
MODULE_LICENSE("GPL");

