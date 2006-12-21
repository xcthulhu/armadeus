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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2002 Freescale Semiconductors HK Ltd
 *
 */

/******************************************************************************
 * Copyright (C) 2001, All Rights Reserved
 *
 * File Name: pwm.c
 *
 *
 * Date of Creations:  Mar 6, 2004
 *
 * Synopsis:
 *
 * Descirption:
 * 	Driver for the DB-MX2 PWM, original from driver source for DB-MX1 PWM
 *
 * Modification History:
 *
 * ****************************************************************************/

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

#include <asm/irq.h>
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>

// For register adresses
#include <asm/arch/imx-regs.h>

#include "pwm.h"

#define PWM_TEST

#define DRIVER_NAME        "pwm"

#define DEV_IRQ_NAME    DRIVER_NAME
#define DEV_IRQ_ID      DRIVER_NAME

#define MAX_ID          0x14

#define	_reg(REG)       (*((volatile unsigned long *)(REG)))

//#define	PK(fmt, args...)	printk(fmt, ## args)
#define	PK(...)

#define	PWM_FIFO_AV     0x20
#define	PWM_IRQ_AV      0x80


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
	int		period;
	void		(*timer_func)(unsigned long);
	int		stop_flag;
}PWM_Timer_t;

static PWM_Timer_t pwmTimer;
static timer_blk_t timer_blk;

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
static void soft_reset (void)
{
//    int	i;
    
    //enable the PWM
    PWMC |= 0x00000010;
    
    // set SWR bit to 1
    PWMC |= 0x00010000;
    
    //disable the PWM
    PWMC &= ~0x00000010;
    
    //delay because PWM released after 5 system clock
    udelay(5);
//     for(i=0;i<5;i++)
//         delay();
}

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

	PWMC &= ~0x00000040;	// disable IRQEN
	PWMC &= ~0x00000010;	// disable EN

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

static void pwmIntFunc(void)
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


static int handle_pm_event(struct pm_dev *dev, pm_request_t rqst, void *data)
{
	switch(rqst)
	{
		case PM_SUSPEND:
			PK("<1> PM suspend event received in PWM device.\n");
			StopPwm();

			break;
		case PM_RESUME:
			PK("<1> PM resume event received in PWM device.\n");
			break;
		default:
			break;
	}
    return 0;
} /* handle_pm_event */

/******************************************************************************
 * Function Name: initpwm
 *
 * Input: 			:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: initialize PWM hardware
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/

static int initpwm(void)
{
    //	Init Port PA[2] : PWMO
    //PCCR1 |= 0x10000000; PA2_PF_PWM0;
    // Should be done in apf9328.c non ???
    
    DDIR(0) |=  0x00000002;
    GIUS(0) &= ~0x00000002;
    GPR(0)  &= ~0x00000002;	
    
    return 1;
}


/******************************************************************************
 * Function Name: pwm_release
 *
 * Input: 		inode	:
 * 			filp	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: release resource when close the inode
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
int pwm_release(struct inode * inode, struct file * filp)
{
	// wait unit gWriteCnt == 0
	interruptible_sleep_on(&ts_wait);

    printk(DRIVER_NAME " unloaded successfully\n");
	return 0;
}


/******************************************************************************
 * Function Name: pwm_open
 *
 * Input: 		inode	:
 * 			filp	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: allocate resource when open the inode
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
int pwm_open(struct inode * inode, struct file * filp)
{
    PK("<1>in pwm_open ----\n");
    
    // Init PWM hardware
    initpwm();
    gWriteOk = 0;
    
    printk(DRIVER_NAME " opened \n");
    return 0;
}


/******************************************************************************
 * Function Name: pwm_fasync
 *
 * Input: 		fd	:
 * 			filp	:
 * 			mode	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: provide fasync functionality for select system call
 *
 * Modification History:
 *****************************************************************************/
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

/******************************************************************************
 * Function Name: pwm_ioctl
 *
 * Input: 		inode	:
 * 			filp	:
 * 			cmd	: command for ioctl
 * 			arg	: parameter for command
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: ioctl for this device driver
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
int pwm_ioctl(struct inode * inode,
		struct file *filp,
		unsigned int cmd ,
		unsigned long arg)
{
	char *str=NULL;
	int ret = 0;
	PK("<1>in pwm_ioctl ----\n");

	switch(cmd)
	{
		case PWM_IOC_SMODE:
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
			break;

		case PWM_IOC_SFREQ:
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
			break;

		case PWM_IOC_SDATALEN:
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

/******************************************************************************
 * Function Name: pwm_interrupt
 *
 *
 * Description: interrupt handler
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
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
	PK("out pwm_interrupt ----\n");
    return(IRQ_HANDLED);
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


static struct sysdev_class pwm_sysclass = {
    set_kset_name("pwm"),
};

static struct sys_device pwm_sys_device = {
    .id		= 0,
    .cls		= &pwm_sysclass,
};

static struct class pwm_class = {
    .name		= "pwm",
    .release	= pwm_class_release,
};

/**
 * 
 *
 */
int __init pwm_sysfs_init(void)
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

    //return platform_driver_register(&pxa27x_pwm_driver);

out_sysdev_register:
    sysdev_class_unregister(&pwm_sysclass);
out_sysdev_class:
    class_unregister(&pwm_class);
    return ret;
}

/******************************************************************************
 * Function Name: pwm_init
 *
 * Input: 		void	:
 * Value Returned:	int	: Return status.If no error, return 0.
 *
 * Description: device driver initialization
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
//  static devfs_handle_t devfs_handle;
int __init pwm_init(void)
{
	int result;

	printk("PWM driver version 0.2.0\n");
	PK("<1>Init module\n");

	/* register our character device */

 	result = register_chrdev(0, DRIVER_NAME, &pwm_fops);
 	if ( result < 0 )
 	{
		printk("pwm driver: Unable to register driver\n");
		return result;
	}
/*	devfs_handle = devfs_register(NULL, "pwm", DEVFS_FL_DEFAULT,
				      result, 0,
				      S_IFCHR | S_IRUSR | S_IWUSR,
				      &pwm_fops, NULL);   */
                      
    // Dynamic Major allocation
	if( gMajor == 0 )
	{
		gMajor = result;
		printk("PWM major number = %d\n",gMajor);
	}

	printk("Request IRQs for PWM_INT.\n");
	result = request_irq(PWM_INT,
 			pwm_interrupt,
			SA_INTERRUPT,
			DEV_IRQ_NAME,
			DEV_IRQ_ID);
	if (result)
	{
		printk("<1>cannot init major= %d irq=%d\n", gMajor, irq);

		unregister_chrdev(gMajor, DRIVER_NAME);
/*		devfs_unregister(devfs_handle);			*/
		return -1;
	}

	// init wait queue
	init_waitqueue_head(&ts_wait);

	// register power manager
	//pm_register(PM_SYS_DEV, PM_SYS_UNKNOWN, handle_pm_event);

	// init
	initpwm();

	PK("<1>Init module Complete\n");
	return 0;
}


/******************************************************************************
 * Function Name: pwm_cleanup
 *
 * Input: 		void	:
 * Value Returned:	void	:
 *
 * Description: clean up and free all of resource for this MODULE
 *
 * Modification History:
 * 	10 DEC,2001, 
 *****************************************************************************/
void __exit pwm_cleanup(void)
{
    free_irq(PWM_INT, DEV_IRQ_ID);

    unregister_chrdev(gMajor, DRIVER_NAME);
// 	devfs_unregister(devfs_handle);	

    //platform_driver_unregister(&pxa27x_pwm_driver);
    sysdev_unregister(&pwm_sys_device);
    sysdev_class_unregister(&pwm_sysclass);
    class_unregister(&pwm_class);

    disable_irq(PWM_INT); // should be done before free_irq non ?
}


module_init(pwm_init);
module_exit(pwm_cleanup);
MODULE_AUTHOR("Julien Boibessot");
MODULE_DESCRIPTION("Armadeus PWM driver");
MODULE_LICENSE("GPL");

