/*
 **********************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 **********************************************************************
 */

#define GPIO_PROC_FILE 1
#define SETTINGS_PROC_FILE 4

#include "generic.h"
#include <asm/arch/imx-regs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#define PORT_A      0
#define PORT_B      1
#define PORT_C      2
#define PORT_D      3
#define NB_PORTS    4
#define PORT_MAX_ID 4

enum {
    DDIR_I = 0,
    OCR1_I,
    OCR2_I,
    ICONFA1_I,
    ICONFA2_I,
    ICONFB1_I,
    ICONFB2_I,
    DR_I,
    GIUS_I,
    SSR_I,
    ICR1_I,
    ICR2_I,
    IMR_I,
    GPR_I,
    SWR_I,
    PUEN_I,
};

#define PORTB27_21MASK    ((unsigned long)0x0FF00000)
#define PORTB27_21SHIFT   20
#define PORT_D_31_10_MASK 0xFFFFFC00

// Global variables
struct gpio_operations *driver_ops;
static int gpio_major =  GPIO_MAJOR;

static int number_of_pins[4] = {32, 32, 32, 32};

static unsigned long init_map;
struct semaphore gpio_sema;

static unsigned int gPortAIndex = PORT_A;
static unsigned int gPortBIndex = PORT_B;
static unsigned int gPortCIndex = PORT_C;
static unsigned int gPortDIndex = PORT_D;
// Module parameters

static void __exit armadeus_generic_cleanup(void);


//------------------------------------------------
//
// Low level functions
//
#define DEFAULT_VALUE 0x12345678
static unsigned long MASK[]= { 0x0003FFFE, 0xF00FFF00, 0x0003E1F8, 0xFFFFFFFF };
#define PORT_A_MASK    0x0003FFFE
#define PORT_B_MASK    0xF00FFF00
#define PORT_C_MASK    0x0003E1F8
#define PORT_D_MASK    0xFFFFFFFF


//------------------------------------------------
//
// Handles write() done on /dev/gpioxx
//
static ssize_t armadeus_gpio_write(struct file *file, const char *data, size_t count, loff_t *ppos)
{
    unsigned int minor;
    size_t        i;
    /*char*/unsigned int          port_value=0;
    ssize_t       ret = 0;
    unsigned int  value=0;
    
    minor = MINOR(file->f_dentry->d_inode->i_rdev);
    printk("armadeus_gpio_write() on minor %d\n", minor);
    
    if (ppos != &file->f_pos)
        return -ESPIPE;
    
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;

    value = 0;
    for (i = 0; i < count; ++i) 
    {
        port_value = 0;
        // Get value to write from user space
        ret = __get_user(port_value, (unsigned int*)/*(char *)*/data);
        if (ret != 0) {
            //ret = -EFAULT;
            goto out;
        }

        //writeOnPort( minor, port_value );
        
        data++;
        *ppos = *ppos+1;
    }
    ret = count;

out:
    up(&gpio_sema);
    return ret;
}

//
// Handles read() done on /dev/gpioxx
//
static ssize_t armadeus_gpio_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    unsigned minor = MINOR(file->f_dentry->d_inode->i_rdev);
    unsigned long value=0;
    size_t bytes_read=0;
    ssize_t ret = 0;
    char port_status;
    
    if (count == 0)
        return bytes_read;
    
    if (ppos != &file->f_pos)
        return -ESPIPE;
    
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;
        
    printk("armadeus_gpio_read on minor %d\n", minor);
            
    //value = readFromPort( minor );
    port_status = (char)(value & 0xFF);
    
    if (copy_to_user(buf, &port_status, sizeof(unsigned int))) {
        ret = -EFAULT;
        goto out;
    }
    bytes_read += sizeof(unsigned int);
    buf += sizeof(unsigned int);
    *ppos = *ppos+4;
    ret = bytes_read;

out:
    up(&gpio_sema);
    return ret;
}

static int armadeus_gpio_open(struct inode *inode, struct file *file)
{
    unsigned minor = MINOR(inode->i_rdev);
    
    if( minor >= PORT_MAX_ID ) {
        printk("unsupported minor\n");
        return -EINVAL;
    }

    printk("Opening /dev/gpio%d file\n", minor);
    return 0;
}

static int armadeus_gpio_release(struct inode *inode, struct file *file)
{
    unsigned minor = MINOR(inode->i_rdev);
    
    printk("Closing access to /dev/gpio%d\n", minor);
    return 0;
}


//------------------------------------------------
// PROC file functions
//
static int procfile_gpio_read( char *buffer, __attribute__ ((unused)) char **start, off_t offset, int buffer_length, int *eof, void* data) 
{
    int len; /* The number of bytes actually used */
    unsigned int port_status=0x53;
    unsigned int port_ID = 0;
    
    if( data != NULL )
    {   
        port_ID = *((unsigned int*)data);
        //printk("procfile_gpio_read %d \n", port_ID);
    }
    
    // We give all of our information in one go, so if the user asks us if we have more information the answer should always be no.
    // This is important because the standard read function from the library would continue to issue the read system call until 
    // the kernel replies that it has no more information, or until its buffer is filled.
    
    if( (offset > 0) || (buffer_length < number_of_pins[port_ID]+2) ) 
    {
        return 0;
    }
    
    len = buffer_length;
    if( len > MAX_NUMBER_OF_PINS + 1 ) 
    {
        len = MAX_NUMBER_OF_PINS + 1;
    }
    
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;
    
    // Get the status of the gpio ports
    //port_status = readFromPort( port_ID );
    // Put status to given chr buffer
    //len = toString(port_status, buffer, number_of_pins[/*PORT_B*/port_ID]);
    
    //*start = buffer;
    *eof = 1;
    up(&gpio_sema);
    
    // Return the length    
    return len;
}


static int procfile_gpio_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, void *data)
{
    int len;
    char new_gpio_state[MAX_NUMBER_OF_PINS+1];
    unsigned int gpio_state = 0;
    unsigned int port_ID = 0;
    
    if( data != NULL )
    {   
        port_ID = *((unsigned int*)data);
    }

    // Do some checks on parameters    
    if( count <= 0 ){
        printk("Empty string transmitted !\n");
        return 0;
    }    
    if( count > (MAX_NUMBER_OF_PINS + 1) ) {
        len = MAX_NUMBER_OF_PINS;
       printk("Gpio port is only 32bits !\n");
    } else {
        len = count;
    }

    // Get exclusive access to port    
    if( down_interruptible(&gpio_sema) )
        return -ERESTARTSYS;
    
    // Get datas to write from user space
    if( copy_from_user(new_gpio_state, buf, len) ) {
        up(&gpio_sema);
        return -EFAULT;
    }
    
    // Convert it from String to Int
    //gpio_state = fromString(new_gpio_state, number_of_pins[port_ID] /*__number_of_pins*/);
    printk("/proc asking me to write %d bits (0x%x) on GPIO%d\n", len, gpio_state, port_ID);

    // Put value in GPIO registers    
    //writeOnPort( port_ID, gpio_state );
    
    up(&gpio_sema);
    
    return len;
}


//------------------------------------------------
//  Handling of IOCTL calls 
//
int armadeus_generic_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
    int err = 0; int ret = 0;
    int value=0;
    unsigned int minor;
    
    printk(" ## IOCTL received: (0x%x) ##\n", cmd);
    
    // Extract the type and number bitfields, and don't decode wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
    if (_IOC_TYPE(cmd) != PP_IOCTL) return -ENOTTY;

    // The direction is a bitmask, and VERIFY_WRITE catches R/W transfers. `Type' is user-oriented, while access_ok is kernel-oriented, 
    // so the concept of "read" and "write" is reversed
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));

    if (err) return -EFAULT;

    // Obtain exclusive access
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;
    // Extract and test minor
    minor = MINOR(inode->i_rdev);
    if( minor > GPIO_MAX_MINOR ) {
        printk("Minor outside range: %d !\n", minor);
        return -EFAULT;
    }
    
    switch(cmd) 
    {
        case GPIORDDIRECTION:
            //value = getPortDir( minor );
            //    value = (getPortDir( PORT_B ) & PORTB27_21MASK) >> PORTB27_21SHIFT;
            ret = __put_user(value, (unsigned int *)arg);
        break;

        case GPIOWRDIRECTION:
            ret = __get_user(value, (unsigned int *)arg);

            if( ret==0 ) {
                //setPortDir( minor, value );
            }
        break;

        case GPIORDDATA:
            //value = readFromPort( minor );
            ret = __put_user(value, (unsigned int *)arg);
        break;

        case GPIOWRDATA:
            ret = __get_user(value, (unsigned int *)arg);
            if( ret == 0 ) {
                //writeOnPort( minor, value );
            }
        break;

        default:
            return -ENOTTY;
        break;
    }
    // Release exclusive access
    up(&gpio_sema);

    return ret;
}

//
// Create /proc entries for direct access (with echo/cat) to GPIOs config
//
static int create_proc_entries( void )
{
    static struct proc_dir_entry *Proc_PortA;
    
    //
    printk("Creating /proc entries: ");
    // Create main directory
    proc_mkdir(GPIO_PROC_DIRNAME, NULL);
    // Create proc file to handle GPIO values
    Proc_PortA = create_proc_entry( GENERIC_PROC_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    
    if( Proc_PortA == NULL ) 
    {
        printk(KERN_ERR DRIVER_NAME ": Could not register a " GENERIC_PROC_FILENAME  ". Terminating\n");
        armadeus_generic_cleanup();
        return -ENOMEM;
    } 
    else 
    {
        Proc_PortA->read_proc  = procfile_gpio_read;   Proc_PortA->write_proc = procfile_gpio_write; Proc_PortA->data  = (void*)&gPortAIndex;
        init_map |= GPIO_PROC_FILE;
    }
    
    printk("OK!\n");
    return(0);
}

static struct file_operations gpio_fops = {
    .owner   = THIS_MODULE,
    .write   = armadeus_gpio_write,
    .read    = armadeus_gpio_read,
    .open    = armadeus_gpio_open,
    .release = armadeus_gpio_release,
    .ioctl   = armadeus_generic_ioctl,
};

//
// Module's initialization function
//
static int __init armadeus_generic_init(void)
{
    static int result, i;

    printk("Initializing Armadeus generic driver\n");
    init_map = 0;
       
    // Register the driver by getting a major number
    result = register_chrdev(gpio_major, DRIVER_NAME, &gpio_fops);
    if (result < 0) 
    {
        printk(KERN_WARNING DRIVER_NAME ": can't get major %d\n", gpio_major);
        return result;
    }
    if( gpio_major == 0 ) gpio_major = result; // dynamic Major allocation

    // Creating /proc entries
    result = create_proc_entries();
    if( result < 0 ) return( result );
    
    // Initialise GPIO port access semaphore
    sema_init(&gpio_sema, 1);

    // Set GPIOs to initial state
    //??
    
    printk( DRIVER_NAME " " DRIVER_VERSION " successfully loaded !\n");

    return(0);
}

//
// Module's cleanup function
//
static void __exit armadeus_generic_cleanup(void)
{
    printk( DRIVER_NAME " Cleanup: ");
    
    // Remove /proc entries
    remove_proc_entry( GENERIC_PROC_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTB_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTC_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTD_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTADIR_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTBDIR_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTCDIR_FILENAME, NULL);
//     remove_proc_entry( GPIO_PROC_PORTDDIR_FILENAME, NULL);

    // De-register from /dev interface
    unregister_chrdev(gpio_major, DRIVER_NAME);

    printk("Ok !\n ");
}

module_init(armadeus_generic_init);
module_exit(armadeus_generic_cleanup);
MODULE_AUTHOR("Julien Boibessot");
MODULE_DESCRIPTION("Armadeus generic driver");
MODULE_LICENSE("GPL");
