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

#include "core.h"
#include <asm/arch/imx-regs.h>


#define PORT_A      0
#define PORT_B      1
#define PORT_C      2
#define PORT_D      3
#define PORT_MAX_ID 4

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
static unsigned long port_masks[4] = { 0, 0, 0, 0 };
MODULE_PARM( port_masks, "4i" );

void __exit armadeus_gpio_cleanup(void);


int toString(unsigned long value, char* buffer, int number_of_bits) 
{
    static int i;
    
    /* convert it into a string */
    for(i=number_of_bits;i>0;i--){
        buffer[number_of_bits-i]=test_bit(i-1,&value)?'1':'0';
    }
    
    buffer[number_of_bits] = '\n';
    buffer[number_of_bits+1] = 0;
    
    return number_of_bits+1;
}

unsigned long fromString(char* buffer, int number_of_bits) 
{
    static int i;
    static unsigned long ret_val=0;
    
    ret_val = 0;
    /* Create WORD to write from the string */
    for(i=0;i<number_of_bits;i++)
    {
        if (buffer[i] == 0) break;
    
        if (buffer[i] == '0' || buffer[i] == '1') {
            ret_val=ret_val<<1;
        }
    
        if (buffer[i] == '1') {
            ret_val |= 1;
        }
    }
    
    return(ret_val);
}

//------------------------------------------------
//
// Low level functions
//
static void initializePorts( void )
{
    GIUS(PORT_B) = GIUS(PORT_B) | PORTB27_21MASK; //set only portb27..21
    PUEN(PORT_B) = PUEN(PORT_B) | PORTB27_21MASK;
    OCR1(PORT_B) = OCR1(PORT_B); // nothing to do for 27..21
    OCR2(PORT_B) = OCR2(PORT_B) | 0x00FFFF00;
    
    GIUS(PORT_D) = GIUS(PORT_D) | PORT_D_31_10_MASK; //set only portD 31..10
    PUEN(PORT_D) = PUEN(PORT_D) | PORT_D_31_10_MASK;
    OCR1(PORT_D) = OCR1(PORT_D) | 0xFFFF0000;
    OCR2(PORT_D) = OCR2(PORT_D) | 0xFFFFFFFF;
}

static void writeOnPort( unsigned int aPort, unsigned int aValue )
{
     DR(aPort) = (aValue & 0xffffffff) /*<< PORTB27_21SHIFT*/;
}

static unsigned int readFromPort( unsigned int aPort )
{
    unsigned int port_value = 0;
    
    // Get the status of the gpio ports TBDNICO
    port_value = (SSR(aPort));
    
    return( port_value );
}

static void setPortDir( unsigned int aPort, unsigned int aDirMask )
{
    DDIR(aPort) = (aDirMask & 0xffffffff);
}

static unsigned int getPortDir( unsigned int aPort )
{
    unsigned int port_value = 0;
    
    // Get the status of the gpio direction registers TBDNICO
    port_value = (DDIR(aPort));
    
    return( port_value );
}


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

        writeOnPort( minor, port_value );
        
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
            
    value = readFromPort( minor );
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
    port_status = readFromPort( port_ID );
    
    // Put status to given chr buffer
    len = toString(port_status, buffer, number_of_pins[/*PORT_B*/port_ID]);
    
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
    unsigned int gpio_state;
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
    gpio_state = fromString(new_gpio_state, number_of_pins[port_ID] /*__number_of_pins*/);
    printk("/proc asking me to write %d bits (0x%x) on GPIO%d\n", len, gpio_state, port_ID);

    // Put value in GPIO registers    
    writeOnPort( port_ID, gpio_state );
    
    up(&gpio_sema);
    
    return len;
}

static int procfile_settings_read( char *buffer, char **start, off_t offset, int buffer_length, int *eof, __attribute__ ((unused)) void *data )
{
    int len; /* The number of bytes actually used */
    unsigned int port_status = 0x66;
    unsigned int port_ID = 0;
    
    if( data != NULL )
    {   
        port_ID = *((unsigned int*)data);
    }
    
    if( offset > 0 || buffer_length < (number_of_pins[PORT_B]+2) )
        return 0;
    
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;
    
    // Get the status of the gpio direction registers TBDNICO
    port_status = getPortDir(port_ID);
    //printk("procfile_settings_read (0x%x)\n", count, gpio_state);
    // Put status to given chr buffer
    len = toString( port_status,buffer, number_of_pins[port_ID] );
    
    //*start = buffer;
    *eof = 1;
    up(&gpio_sema);
    
    // Return the length    
    return len;
}

static int procfile_settings_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, __attribute__ ((unused)) void* data)
{
    int len;
    int p1,p2;
    char ch;
    char new_gpio_state[MAX_NUMBER_OF_PINS+1];
    unsigned int  gpio_state;
    unsigned int port_ID = 0;
    
    if( data != NULL )
    {   
        port_ID = *((unsigned int*)data);
    }
    
    if (count==0) return 0;
    
    if (down_interruptible(&gpio_sema))
        return -ERESTARTSYS;
    
    if( count > (MAX_NUMBER_OF_PINS + 1) ) {
        len = MAX_NUMBER_OF_PINS;
       printk("Gpio port setting register is only 32bits !\n");
    } else {
        len = count;
    }
    
    // Get datas to write from user space
    if(copy_from_user(new_gpio_state, buf, len)) {
        up(&gpio_sema);
        return -EFAULT;
    }
    
    /* make sure our string only contains 1 and 0 */
    p1=0;
    p2=0;
    while (p2<=len) 
    {
        ch=new_gpio_state[p2];
    
        if (ch == '0' || ch == '1' || ch == '\0') {
            new_gpio_state[p1] = ch;
            p1++;
        }
    
        if (ch==0) break;
            p2++;
    }
    new_gpio_state[p1] = 0;
    
    if( strlen(new_gpio_state) > 0 ) 
    {
        // Convert it from String to Int
        gpio_state = fromString( new_gpio_state, number_of_pins[port_ID] );
        printk("/proc asking me to write %d bits (0x%x) on GPIO settings\n", len, gpio_state);
       // 
        setPortDir( port_ID, gpio_state );
    }
    
    up(&gpio_sema);
    return len;
}


//------------------------------------------------
//  Handling of IOCTL calls 
//
int armadeus_gpio_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
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
            value = getPortDir( minor );
            //    value = (getPortDir( PORT_B ) & PORTB27_21MASK) >> PORTB27_21SHIFT;
            ret = __put_user(value, (unsigned int *)arg);
        break;

        case GPIOWRDIRECTION:
            ret = __get_user(value, (unsigned int *)arg);

            if( ret==0 ) {
                setPortDir( minor, value );
            }
        break;

        case GPIORDDATA:
            value = readFromPort( minor );
            ret = __put_user(value, (unsigned int *)arg);
        break;

        case GPIOWRDATA:
            ret = __get_user(value, (unsigned int *)arg);
            if( ret == 0 ) {
                writeOnPort( minor, value );
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
    static struct proc_dir_entry *Proc_PortA, *Proc_PortB, *Proc_PortC, *Proc_PortD;
    static struct proc_dir_entry *Proc_PortAdir, *Proc_PortBdir, *Proc_PortCdir, *Proc_PortDdir;
    
    //
    printk("Creating /proc entries: ");
    // Create main directory
    proc_mkdir(GPIO_PROC_DIRNAME, NULL);
    // Create proc file to handle GPIO values
    Proc_PortA = create_proc_entry( GPIO_PROC_PORTA_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortB = create_proc_entry( GPIO_PROC_PORTB_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortC = create_proc_entry( GPIO_PROC_PORTC_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortD = create_proc_entry( GPIO_PROC_PORTD_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);

    if( (Proc_PortA == NULL) || (Proc_PortB == NULL) ) 
    {
        printk(KERN_ERR OUR_NAME ": Could not register a " GPIO_PROC_PORTA_FILENAME  ". Terminating\n");
        armadeus_gpio_cleanup();
        return -ENOMEM;
    } 
    else 
    {
        Proc_PortA->read_proc  = procfile_gpio_read;   Proc_PortA->write_proc = procfile_gpio_write; Proc_PortA->data  = (void*)&gPortAIndex;
        Proc_PortB->read_proc  = procfile_gpio_read;   Proc_PortB->write_proc = procfile_gpio_write; Proc_PortB->data  = (void*)&gPortBIndex;
        Proc_PortC->read_proc  = procfile_gpio_read;   Proc_PortC->write_proc = procfile_gpio_write; Proc_PortC->data  = (void*)&gPortCIndex;
        Proc_PortD->read_proc  = procfile_gpio_read;   Proc_PortD->write_proc = procfile_gpio_write; Proc_PortD->data  = (void*)&gPortDIndex;
        
        init_map |= GPIO_PROC_FILE;
    }
    
    // Create proc file to handle GPIO direction settings
    Proc_PortAdir = create_proc_entry( GPIO_PROC_PORTADIR_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortBdir = create_proc_entry( GPIO_PROC_PORTBDIR_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortCdir = create_proc_entry( GPIO_PROC_PORTCDIR_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);
    Proc_PortDdir = create_proc_entry( GPIO_PROC_PORTDDIR_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL);

    if( (Proc_PortAdir == NULL) || (Proc_PortBdir == NULL) ) 
    {
        printk(KERN_ERR OUR_NAME ": Could not register " GPIO_PROC_PORTADIR_FILENAME ". Terminating\n");
        armadeus_gpio_cleanup();
        return -ENOMEM;
    } 
    else 
    {
        Proc_PortAdir->read_proc = procfile_settings_read; Proc_PortAdir->write_proc = procfile_settings_write; 
        Proc_PortAdir->data  = (void*)&gPortAIndex;
        Proc_PortBdir->read_proc = procfile_settings_read; Proc_PortBdir->write_proc = procfile_settings_write; 
        Proc_PortBdir->data  = (void*)&gPortBIndex;
        Proc_PortCdir->read_proc = procfile_settings_read; Proc_PortCdir->write_proc = procfile_settings_write; 
        Proc_PortCdir->data  = (void*)&gPortCIndex;
        Proc_PortDdir->read_proc = procfile_settings_read; Proc_PortDdir->write_proc = procfile_settings_write; 
        Proc_PortDdir->data  = (void*)&gPortDIndex;
        init_map |= SETTINGS_PROC_FILE;
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
    .ioctl   = armadeus_gpio_ioctl,
};

//
// Module's initialization function
//
int __init armadeus_gpio_init(void)
{
    static int result;

    printk("Initializing Armadeus GPIOs driver\n");
    printk("    Parameters received: %x %x %x %x\n", port_masks[0], port_masks[1], port_masks[2], port_masks[3] );
    init_map = 0;
    
    // Configure HW ports/GPIOs with default values or given parameters
    initializePorts();
    
    // Register the driver by getting a major number
    result = register_chrdev(gpio_major, OUR_NAME, &gpio_fops);
    if (result < 0) 
    {
        printk(KERN_WARNING OUR_NAME ": can't get major %d\n", gpio_major);
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
    
    printk("Armadeus GPIOs driver " DRIVER_VERSION " successfully loaded !\n");

    return(0);
}

//
// Module's cleanup function
//
void __exit armadeus_gpio_cleanup(void)
{
    printk("Cleanup: ");
    
    // Remove /proc entries
    remove_proc_entry( GPIO_PROC_PORTA_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTB_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTC_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTD_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTADIR_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTBDIR_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTCDIR_FILENAME, NULL);
    remove_proc_entry( GPIO_PROC_PORTDDIR_FILENAME, NULL);

    // De-register from /dev interface
    unregister_chrdev(gpio_major, OUR_NAME);

    printk("Ok !\n ");
}

//------------------------------------------------
//
// API
//

void gpioWriteOnPort( unsigned int aPort, unsigned int aValue )
{
    writeOnPort( aPort, aValue );
}

unsigned int gpioReadFromPort( unsigned int aPort )
{
    return( readFromPort( aPort ) );
}

void gpioSetPortDir( unsigned int aPort, unsigned int aDirMask )
{
    setPortDir( aPort, aDirMask );
}

unsigned int gpioGetPortDir( unsigned int aPort )
{
    return( getPortDir( aPort ) );
}

EXPORT_SYMBOL( gpioWriteOnPort );
EXPORT_SYMBOL( gpioReadFromPort );
EXPORT_SYMBOL( gpioSetPortDir );
EXPORT_SYMBOL( gpioGetPortDir );

module_init(armadeus_gpio_init);
module_exit(armadeus_gpio_cleanup);
MODULE_AUTHOR("JB / NC");
MODULE_DESCRIPTION("Armadeus GPIOs driver");
MODULE_LICENSE("GPL");
