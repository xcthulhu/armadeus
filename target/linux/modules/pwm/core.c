/*
 *
 * Driver for the i.MXl PWM driver
 *
 * This driver is part of the Armadeus project and is the merge of a lot of ideas
 * found on Internet.
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
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/in.h>
//#include <linux/malloc.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <linux/errno.h>
//#include <linux/tqueue.h>
#include <linux/wait.h>
#include <linux/pm.h>
// For /sys
#include <linux/sysdev.h>
// For struct class
#include <linux/device.h>

#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>
// For register adresses
#include <asm/arch/imx-regs.h>
#define PWMC_PRESCALER_MASK (0x7F << 8)  // not defined in imx-regs ?
//
#include <linux/platform_device.h>

#include "pwm.h"

#define PWM_TEST

#define DRIVER_NAME         "imx-pwm"
#define DRIVER_VERSION      "0.4"

#define DEV_IRQ_NAME    DRIVER_NAME
#define DEV_IRQ_ID      DRIVER_NAME

//#define	PK(fmt, args...)	printk(fmt, ## args)
#define	PK(...)

#define	PWM_FIFO_AV     0x20
#define	PWM_IRQ_AV      0x80

// def in linux/compiler.h
//#define unlikely(TOTO) (TOTO > 0)


struct pwm_device {
    u32 reg;
    int active;
    unsigned duty;
    /*const*/ struct pwm_table *entry;
    struct class_device class_dev;
};

static struct class pwm_class;	/* forward declaration only */

#define to_pwm_device(d) container_of(d, struct pwm_device, class_dev)


// variables
int     gWriteOk;
int     gPwmMode;
char	*gpWriteBuf;

u8	*gpWriteBufPtr8;
u16	*gpWriteBufPtr16;
int     gWriteCnt;
int     gDataLen;
u16  gSampleValue;

int     irq = 0;
module_param(irq, int, 0);

wait_queue_head_t ts_wait;
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


// -----------------------
//  FUNCTION ROUTINES
// -----------------------

//delay for a while
// static void delay (void)
// {
// 	u32 i=0;
// 	for (i=0; i<10000; i++);
// }

//software reset
// static void soft_reset (void)
// {
// //    int	i;
//     
//     //enable the PWM
//     PWMC |= 0x00000010;
//     
//     // set SWR bit to 1
//     PWMC |= 0x00010000;
//     
//     //disable the PWM
//     PWMC &= ~0x00000010;
//     
//     //delay because PWM released after 5 system clock
//     udelay(5);
// //     for(i=0;i<5;i++)
// //         delay();
// }

// --------------------------------------------------------------------
// local rutines

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
    if(gpWriteBuf)
            kfree(gpWriteBuf);

    PWMC &= ~PWMC_IRQEN;    // disable IRQ
    PWMC &= ~PWMC_EN;       // disable PWM

    gWriteOk = 1;
    gWriteCnt = 0;
    gDataLen = 0;
    gSampleValue = 0;

    if(gPwmMode == PWM_TONE_MODE)
            StopTimer(&pwmTimer);

    // can release PWM now
    wake_up_interruptible(&ts_wait);

    PK("<1>data completed.\n");
    PK("<1>PWMC = 0x%8x\n", (u32)PWMC);
}

static void pwmIntFunc( unsigned long unused)
{
    u32	period;

    PK("<1>pwm sam int\n");

    if (gWriteCnt > 0)
    {
        PK("<1>PWMS = 0x%4x\n", (u16)_reg_PWM_PWMS);

        // 96M/128/2/period
        period = *gpWriteBufPtr16++;

        if(period > 0)
//			PWMP = 96000000/128/12/2/period; //96000000/128/2*11
            PWMP = 66000000/128/2/period; //96000000/128/2*11
        PK("<1>PWMP = 0x%4x\n", (u16)PWMP);
        PK("<1>PWMCNT = 0x%4x\n", (u16)PWMCNT);

        StartTimer(&pwmTimer);

        gWriteCnt--;
    }
    else
        StopPwm();

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
    //	Init Port PA[2] : PWMO
    //PCCR1 |= 0x10000000; PA2_PF_PWM0;
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

//////////////////////////////////////// /DEV INTERFACE //////////////////////////////////////////////////////////////

/*
 *
 */
int pwm_release(struct inode * inode, struct file * filp)
{
    // wait unit gWriteCnt == 0
    interruptible_sleep_on(&ts_wait);

    printk(DRIVER_NAME " unloaded successfully\n");
    return 0;
}

/*
 *
 */
int pwm_open(struct inode * inode, struct file * filp)
{
    PK("<1>in pwm_open ----\n");

    // Init PWM hardware
    initPWM();
    gWriteOk = 0;

    printk(DRIVER_NAME " opened \n");
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
 *
 */
int pwm_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    char *str=NULL;
    int ret = 0;
    PK("<1>in pwm_ioctl ----\n");

    switch(cmd)
    {
        case PWM_IOC_SMODE:
        {
            gPwmMode = arg;

            if (gPwmMode == PWM_TONE_MODE)
            {
                    // create periodic timer when tone mode
                    PK("PWM Tone Mode.\n");
                    pwmTimer.timer_blk_ptr = &timer_blk;
                    pwmTimer.timer_func = pwmIntFunc;
                    CreateTimer(&pwmTimer);
            }
            else
            {
                    PK("PWM Play Mode.\n");
            }
        }
        break;

        case PWM_IOC_SFREQ:
        {
            PWMC &= ~0x00000010; // Disable PWM
            
            PWMC &= 0xFFFF00F0; // sysclk
    

            // check sampling rate
            if (arg == PWM_SAMPLING_8KHZ)
            {
                    // 96M / 23/2/256 = 8.152kHz
                    //				PWMC |= (0 << 8); // pres = 22 (/23)
                    PWMC |= 0x00000900; // pres = 15 (/16) // 66M / 16/2/256 = 8.057KHZ
                    str = "8Khz";
            }

            else if (arg == PWM_SAMPLING_16KHZ)
            {
                    PWMC |= (0 << 8); // pres = 11 (/12))
                    str = "16Khz";
            }

            PWMC &= ~0x3; // clksel = 0 (/2)

            PK("<2>Sample rate = %s\n", str);
            PK("<1>ioctl: PWMC = 0x%8x\n", PWMC);
        }
        break;

        case PWM_IOC_SDATALEN:
        {
            gDataLen = arg;
            if (arg == PWM_DATA_8BIT)
            {
                    PWMP = 0xfe; // Period, 8bit
                    str = "8bit";
            }
            else // if(arg == PWM_DATA_16BIT)
            {
                    PWMP = 0xfffe; // Period, 16bit
                    str = "16bit";
            }

            PK("<2>Data Length = %s\n", str);
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

/*add--001begin*/
        case PWM_IOC_SWAPDATA:
                if (arg&PWM_SWAP_HCTRL) //Halfword FIFO data swapping 
                {
                        PWMC |= PWM_SWAP_HCTRL;
                }
                else
                {
                        PWMC &= ~PWM_SWAP_HCTRL;
                }

                if (arg&PWM_SWAP_BCTRL) //Byte FIFO data swapping 
                {
                        PWMC |= PWM_SWAP_BCTRL;
                }
                else
                {
                        PWMC &= ~PWM_SWAP_BCTRL;
                }			

        break;
/*add--001end*/

        default:
        break;
    }

    PK("<1>out pwm_ioctl ----\n");
    return ret;
}

/******************************************************************************
 * Function Name: pwm_read
 *
 * Input: 		filp	: the file
 * 			buf	: data buffer
 * 			count	: number of chars to be readed
 * 			l	: offset of file
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: read device driver
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
ssize_t pwm_read(struct file * filp, char * buf, size_t count, loff_t * l)
{
    int ret;

    if(gWriteCnt == 0)
        ret = 1; // no data
    else
        ret = 0; // processing data

    printk("pwm_read ----\n");
    return ret;
}

static ssize_t pwm_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos)
{
    int ret = 0;

    PK("<1>in pwm_write ----\n");

    gWriteOk = 0;

    gpWriteBuf = kmalloc(count, GFP_KERNEL);
    if (!gpWriteBuf)
            goto out;
    gpWriteBufPtr8 = gpWriteBuf;

    if (copy_from_user(gpWriteBuf, buf, count))
    {
        ret = -EFAULT;
        goto out;
    }

    //write data to PWM
    gWriteCnt = count;
    PK("<1>pwm_write: count = %d\n", gWriteCnt);

    //enable IRQEN, PW
    PWMC |= 0x00000010;

    PWMS = 0x00000000;

    if(gPwmMode == PWM_PLAY_MODE)
    {
//		PK("before set IRQEN\n");
        PWMC |= 0x00000040;
        PK("PWMC1= 0x%08x\n", PWMC);
//               PWMS = 0x00000000;
    }
    else
    {
        PWMS = gSampleValue;
        PWMC |= 0x7f00;	// set prescaler to 128
        gpWriteBufPtr16 = (u16*)gpWriteBuf;

        gWriteCnt /= 2; //input size is 8bit size, need /2 to get 16bit size

        StartTimer(&pwmTimer);
    }

    PK("<1>pwm_write: PWMC = 0x%8x\n", PWMC);

out:

    PK("<1>out pwm_write ----\n");
    return ret;
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
    int rc;
    struct pwm_device *ppd;

    if (unlikely((ppd = kmalloc(sizeof(struct pwm_device), GFP_KERNEL)) == NULL))
        return -ENOMEM;

    memset(&ppd->class_dev, 0, sizeof(struct class_device));
    ppd->class_dev.class = &pwm_class;
    ppd->class_dev.dev=&pdev->dev;
    snprintf(ppd->class_dev.class_id, BUS_ID_SIZE, "pwm%i", pdev->id);
    ppd->reg=address;
    ppd->entry=NULL;
    ppd->active=0;
    ppd->duty=500; // = 50.O%

    rc = class_device_register(&ppd->class_dev);
    if (unlikely(rc)) {
            printk(KERN_ERR "%s: class registering failed\n", DRIVER_NAME);
            kfree(ppd);
            return rc;
    }
    // Register the attributes
    class_device_create_file( &ppd->class_dev, &attr_pwm_duty );
    class_device_create_file( &ppd->class_dev, &attr_pwm_period );
    class_device_create_file( &ppd->class_dev, &attr_pwm_frequency );
    class_device_create_file( &ppd->class_dev, &attr_pwm_active );

    pdev->dev.driver_data=ppd;

    return 0;
}

/*
 *  PWM interrupt handler
 */
static irqreturn_t pwm_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    u16 tmp16;
    u32 status;

    // clear status
    status = PWMC;

    // check end
    if(gWriteCnt < 0)
    {
        StopPwm();
        goto end;
    }

    if (gPwmMode == PWM_PLAY_MODE)
    {
        if(gDataLen == PWM_DATA_8BIT)
        {
            PWMS = (u32)(*gpWriteBufPtr8++);
            PK("<1>(%d) pwm_int: PWMS = 0x%8x\n", gWriteCnt, PWMS);

            gWriteCnt--;
        }
        else if(gDataLen == PWM_DATA_16BIT)
        {
            tmp16 = *gpWriteBufPtr8++;
            tmp16 |= (*gpWriteBufPtr8++ << 8);
            PWMS = (u32)tmp16;
            PK("<1>(%d) pwm_int: PWMS = 0x%8x\n", gWriteCnt, PWMS);

            gWriteCnt -= 2;
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

    result = request_irq(PWM_INT, pwm_interrupt, SA_INTERRUPT, DEV_IRQ_NAME, DEV_IRQ_ID);
    if (result)
    {
        printk("<1>cannot init major= %d irq=%d\n", gMajor, irq);
        unregister_chrdev(gMajor, DRIVER_NAME);
        goto end;
    }

    // init wait queue
    init_waitqueue_head(&ts_wait);

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

/**
 * imx_pwm_drv_remove - free claimed resources
 * @pdev: platform device to work with
 */
static int imx_pwm_drv_remove (struct platform_device *pdev)
{
    struct resource *res;
    struct pwm_device *ppd=(struct pwm_device*)pdev->dev.driver_data;

    // Unregister /sys attributes
    class_device_remove_file( &ppd->class_dev, &attr_pwm_active );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_period );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_frequency );
    class_device_remove_file( &ppd->class_dev, &attr_pwm_duty   );

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
    disable_irq( PWM_INT ); // should be done before free_irq non ?
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
MODULE_AUTHOR("Julien Boibessot, inspired by a lot of other PWM drivers");
MODULE_DESCRIPTION("Armadeus PWM driver");
MODULE_LICENSE("GPL");

