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

#include "loadfpga.h"
#include <asm/arch/imx-regs.h>

struct fpga_operations *driver_ops;
static int fpga_major =  FPGA_MAJOR;

//static unsigned long init_map;
struct semaphore fpga_sema;
/* global variables */

static unsigned long gParameters[4] = { 0, 0, 0, 0 };
//static unsigned char port = PORT_A;
MODULE_PARM( port, "1i" );

#define FPGA_BUFFER_SIZE 4096
static unsigned char gBuffer[FPGA_BUFFER_SIZE];
static unsigned char gNbOfUsers = 0;

void __exit armadeus_fpga_cleanup(void);


//------------------------------------------------
//
// Low level functions
//
static void initializePorts( void )
{
    // Ask GPIO driver to configure ports needed by this driver
    // TBDNICO
}


//------------------------------------------------
//
// Handles write() done on /dev/fpgaxx
//
static ssize_t armadeus_fpga_write(struct file *file, const char* pData, size_t count, loff_t *f_pos)
{
    unsigned int minor;
    ssize_t       ret = 0;
    
    minor = MINOR(file->f_dentry->d_inode->i_rdev);
    printk("armadeus_fpga_write() on minor %d\n", minor);
    
    if( f_pos != &file->f_pos) {
        ret = -ESPIPE;
        goto out;
    }
    // Get exclusive access
    if (down_interruptible(&fpga_sema))
        return -ERESTARTSYS;

    if( count > FPGA_BUFFER_SIZE ) {
        count = FPGA_BUFFER_SIZE;
        printk("Buffer truncated while writing data on fpga: %d -> %d\n", count, FPGA_BUFFER_SIZE);
    }
    // Get value to write from user space
    ret = __copy_from_user( gBuffer, pData, count);
    if (ret != 0) {
        //ret = -EFAULT;
        goto out;
    }

    // Copy data to fpga
    // TBD NICO
    
    *f_pos += count;
    ret = count;

out:
    // Release exclusive access
    up(&fpga_sema);
    
    return( ret );
}

//
// Handles read() done on /dev/fpgaxx
//
// static ssize_t armadeus_fpga_read(struct file *file, char *buf, size_t count, loff_t *ppos)
// {
//        NOT SUPPORTED
// }


static int armadeus_fpga_open(struct inode *inode, struct file *file)
{
    unsigned minor = MINOR(inode->i_rdev);
    
//     if( minor != MINOR_BYTE && minor !=MINOR_FULL && minor !=MINOR_LED )
//         return( -EINVAL );

    // Only one access at a time is permitted
    if( gNbOfUsers > 0 )
        return( -EBUSY );

    gNbOfUsers++;
    
    printk("Opening /dev/fpga%d file\n", minor);
    return 0;
}

static int armadeus_fpga_release(struct inode *inode, struct file *file)
{
    unsigned minor = MINOR(inode->i_rdev);
    
    printk("Closing access to /dev/fpga%d\n", minor);
    return 0;
}

//------------------------------------------------
// PROC file
//
static int procfile_fpga_read( char *buffer, __attribute__ ((unused)) char **start, off_t offset, int buffer_length, int *eof, __attribute__ ((unused)) void* data) 
{
    int len; /* The number of bytes actually used */
    
    // We give all of our information in one go, so if the user asks us if we have more information the answer should always be no.
    // This is important because the standard read function from the library would continue to issue the read system call until 
    // the kernel replies that it has no more information, or until its buffer is filled.
    
    if( (offset > 0) || (buffer_length < /*MIN_SIZE_OF_PROC*/32) ) 
    {
        return 0;
    }
    
    if (down_interruptible(&fpga_sema))
        return -ERESTARTSYS;
    
    // Get the status of the fpga TBDNICO
        
    // Put status to given buffer
    len = sprintf(buffer, "Ben j'ai trop rien à dire là mais bon ça changera surement :-) !\n");
    
    //*start = buffer;
    *eof = 1;
    
    up(&fpga_sema);
    
    // Return the length    
    return len;
}


static int procfile_fpga_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, void *data)
{
    int len = 0;

    // Do some checks on parameters    
    // Get datas to write from user space
    
    printk( "Not supported !\n" );
    
    return len;
}

//------------------------------------------------
//  Handling of IOCTL calls 
//
int armadeus_fpga_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
    int err = 0; int ret = 0;
    unsigned int minor;
    
    //printk(" ## IOCTL received: (0x%x) ##\n", cmd);
    
    // Extract the type and number bitfields, and don't decode wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
    if (_IOC_TYPE(cmd) != FPGA_IOCTL) return -ENOTTY;

    // The direction is a bitmask, and VERIFY_WRITE catches R/W transfers. `Type' is user-oriented, while access_ok is kernel-oriented, 
    // so the concept of "read" and "write" is reversed
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));

    if (err) return -EFAULT;

    if (down_interruptible(&fpga_sema))
        return -ERESTARTSYS;

    minor = MINOR(inode->i_rdev);
    if( minor > FPGA_MAX_MINOR ) {
        printk("Minor outside range: %d !\n", minor);
        return -EFAULT;
    }
    
    // Handle IOCTL command
    switch(cmd) 
    {
        /*case PPCLAIM:
        case PPRELEASE:
        break;
        case PPFCONTROL: // Normally not usefull in ur cas because we will ask LCD4LINUX to generate control signals with Data ports
            ret = __get_user(value, (unsigned char *)arg);
            Write ctrl infos on assigned ctrl gpiolines how ???
            printk("/FROB 0x%x/ ", value);
        break;*/

        default:
            return -ENOTTY;
        break;
        
    }

    up(&fpga_sema);

    return ret;
}

//
// Create /proc entries for direct access (with echo/cat) to GPIOs config
//
static int createProcEntries( void )
{
    static struct proc_dir_entry *fpga_Proc_File;
    //
    printk("Creating /proc entries: ");
    // Create main directory
    proc_mkdir(FPGA_PROC_DIRNAME, NULL);
    // Create proc file to handle GPIO values
    fpga_Proc_File = create_proc_entry( FPGA_PROC_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL );

    if( fpga_Proc_File == NULL ) 
    {
        printk(FPGA_DRIVER_NAME ": Could not register a" FPGA_PROC_FILENAME  ". Terminating\n");
        armadeus_fpga_cleanup();
        return -ENOMEM;
    } 
    else 
    {
        fpga_Proc_File->read_proc        = procfile_fpga_read;   
        fpga_Proc_File->write_proc       = procfile_fpga_write;
    }
    
    printk("OK!\n");
    return(0);
}

static struct file_operations fpga_fops = {
    .owner   = THIS_MODULE,
    .write   = armadeus_fpga_write,
//    .read    = armadeus_fpga_read,
    .open    = armadeus_fpga_open,
    .release = armadeus_fpga_release,
    .ioctl   = armadeus_fpga_ioctl,
};

//
// Module's initialization function
//
int __init armadeus_fpga_init(void)
{
    static int result;

    printk("Initializing Armadeus fpga driver\n");
    //printk("Parameters received: %x %x %x %x\n", port_masks[0], port_masks[1], port_masks[2], port_masks[3] );
    
    // Configure HW ports/GPIOs with default values or given parameters
    initializePorts();
    
    // Register the driver by getting a major number
    result = register_chrdev(fpga_major, FPGA_DRIVER_NAME, &fpga_fops);
    if (result < 0) 
    {
        printk(KERN_WARNING FPGA_DRIVER_NAME ": can't get major %d\n", fpga_major);
        return result;
    }
    if( fpga_major == 0 ) fpga_major = result; // dynamic Major allocation

    // Creating /proc entries
    result = createProcEntries();
    if( result < 0 ) return( result );
    
    printk(FPGA_DRIVER_NAME " module " FPGA_DRIVER_VERSION " successfully loaded !\n");
    return(0);
}

//
// Module's cleanup function
//
void __exit armadeus_fpga_cleanup(void)
{
    printk("Removing " FPGA_DRIVER_NAME " module: ");

    // Remove /proc entries    
    remove_proc_entry(FPGA_PROC_FILENAME, NULL);
    // De-register /dev interface
    unregister_chrdev(fpga_major, FPGA_DRIVER_NAME);

    printk("Ok !\n ");
}

//------------------------------------------------

module_init(armadeus_fpga_init);
module_exit(armadeus_fpga_cleanup);

MODULE_AUTHOR("JB / NC");
MODULE_DESCRIPTION("Armadeus fpga's loading driver");
MODULE_LICENSE("GPL");
