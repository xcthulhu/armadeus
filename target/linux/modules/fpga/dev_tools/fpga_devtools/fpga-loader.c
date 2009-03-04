/*
 * Generic Xilinx FPGA loader
 *
 * Copyright (C) 2006-2009 Julien Boibessot <julien.boibessot@armadeus.com>
 *                         Nicolas Colombain <nicolas.colombain@armadeus.com>
 *                         Armadeus Project / Armadeus systems
 *
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
 */

// #define DEBUG

#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

#include "fpga-loader.h"
#include "xilinx-fpga-loader.h"

#define DRIVER_VERSION "0.8"
#define DRIVER_NAME    "fpgaloader"


/* global variables */
static int fpga_major =  FPGA_MAJOR;
Xilinx_desc* g_current_desc = NULL;
struct semaphore fpga_sema;

static unsigned char fpga_descriptor = 0; /* use default target_fpga_desc */
module_param( fpga_descriptor, byte, 0 );

#define FPGA_BUFFER_SIZE 4096
static unsigned char g_buffer[FPGA_BUFFER_SIZE];
static unsigned char g_nb_users = 0;


/* Handles write() done on /dev/fpga/loader */
static ssize_t armadeus_fpga_write(struct file *file, const char* pData, size_t count, loff_t *f_pos)
{
	ssize_t ret = 0;

	/* Get exclusive access */
	if (down_interruptible(&fpga_sema))
		return -ERESTARTSYS;

	if( count > FPGA_BUFFER_SIZE ) {
		count = FPGA_BUFFER_SIZE;
	}

	/* Get value to write from user space */
	ret = __copy_from_user( g_buffer, pData, count);
	if (ret != 0) {
		ret = -EFAULT;
		goto out;
	}

	ret = xilinx_load( g_current_desc, g_buffer, count );

out:
	/* Release exclusive access */
	up(&fpga_sema);

	return ret;
}

static int armadeus_fpga_open(struct inode *inode, struct file *file)
{
	int ret;

	/* Get exclusive access */
	if (down_interruptible(&fpga_sema))
		return -ERESTARTSYS;

	/* Only one access at a time is permitted */
	if( g_nb_users > 0 )
		return -EBUSY;

	ret = xilinx_init_load( g_current_desc );
	if(!ret) {
		printk("Starting FPGA download\n");
		g_nb_users++;
	}

	pr_debug("Opening /dev/fpga/loader%d file, %d %d\n", MINOR(inode->i_rdev), fpga_descriptor, ret);

	/* Release exclusive access */
	up(&fpga_sema);

	return ret;
}

static int armadeus_fpga_release(struct inode *inode, struct file *file)
{
	if( xilinx_finish_load(g_current_desc) )
		printk("Failed to load FPGA !\n");

	pr_debug("Closing access to /dev/fpga/loader%d\n", MINOR(inode->i_rdev));

	if( g_nb_users > 0 )
		g_nb_users--;

	return 0;
}


/* PROC file */
static int procfile_fpga_read( char *buffer, __attribute__ ((unused)) char **start, off_t offset, int buffer_length, int *eof, __attribute__ ((unused)) void* data) 
{
	int ret;
	
	/* We give all of our information in one go, so if the user asks us if
	   we have more information the answer should always be no.
	   This is important because the standard read function from the
	   library would continue to issue the read system call until the
	   the kernel replies that it has no more information, or until 
	   its buffer is filled */
	if( offset > 0 ) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		if ( data == NULL ) {
			ret = xilinx_get_descriptor_info( -1, buffer );
		} else {
			ret = xilinx_get_descriptor_info( *((unsigned char*)data), buffer );
		}
	}

	return ret;
}

static int procfile_fpga_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, void *data)
{
	return count;
}

/* Handling of IOCTL calls */
int armadeus_fpga_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
	int err = 0; int ret = 0;
	unsigned int minor;

	/* PRINTF( DRIVER_NAME " ## IOCTL received: (0x%x) ##\n", cmd ); */
	/* Extract the type and number bitfields, and don't decode wrong cmds:
	   return ENOTTY (inappropriate ioctl) before access_ok() */
	if (_IOC_TYPE(cmd) != FPGA_IOCTL) return -ENOTTY;

	/* The direction is a bitmask, and VERIFY_WRITE catches R/W transfers.
	  `Type' is user-oriented, while access_ok is kernel-oriented,
	   so the concept of "read" and "write" is reversed */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));

	if (err) return -EFAULT;

	if (down_interruptible(&fpga_sema))
		return -ERESTARTSYS;

	/* Extract and test minor */
	minor = MINOR(inode->i_rdev);
	if( minor > FPGA_MAX_MINOR ) {
		printk("Minor outside range: %d !\n", minor);
		return -EFAULT;
	}

	switch(cmd)
	{
		default:
			return -ENOTTY;
		break;
	}

	up(&fpga_sema);

	return ret;
}

/* Create /proc entries for direct access to FPGA config */
static int create_proc_entries( void )
{
	static struct proc_dir_entry *fpga_Proc_File;
	
	/* Create main directory */
	proc_mkdir(FPGA_PROC_DIRNAME, NULL);
	
	/* Create proc file */
	fpga_Proc_File = create_proc_entry( FPGA_PROC_FILENAME, S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH, NULL );
	if( fpga_Proc_File == NULL ) {
		printk(DRIVER_NAME ": Could not register a" FPGA_PROC_FILENAME  ". Terminating\n");
		goto error;
	} else {
		fpga_Proc_File->read_proc = procfile_fpga_read;
		fpga_Proc_File->write_proc = procfile_fpga_write;
	}
	
	return 0;
error:
	remove_proc_entry(FPGA_PROC_DIRNAME, NULL);
	return -ENOMEM;
}

static struct file_operations fpga_fops = {
	.owner   = THIS_MODULE,
	.write   = armadeus_fpga_write,
	/*    .read    = armadeus_fpga_read, Configuration saving not supported yet */
	.open    = armadeus_fpga_open,
	.release = armadeus_fpga_release,
	.ioctl   = armadeus_fpga_ioctl,
};

int __init armadeus_fpga_init(void)
{
	static int result;
	
	/* Register the driver by getting a major number */
	result = register_chrdev(fpga_major, DRIVER_NAME, &fpga_fops);
	if (result < 0) {
		printk(KERN_WARNING DRIVER_NAME ": can't get major %d\n", fpga_major);
		return result;
	}
	if( fpga_major == 0 ) fpga_major = result; /* dynamic MAJOR allocation */

	result = create_proc_entries();
	if( result < 0 )
		return result;

	sema_init(&fpga_sema, 1);

	/* initialize the current fpga descriptor with the one by default */
	g_current_desc = xilinx_get_descriptor(fpga_descriptor);
	if( g_current_desc == NULL ){
		return -EINVAL;
	}
	
	printk(DRIVER_NAME " v" DRIVER_VERSION " successfully loaded !\n");
	return 0;
}

void __exit armadeus_fpga_cleanup(void)
{
	remove_proc_entry(FPGA_PROC_FILENAME, NULL);
	remove_proc_entry(FPGA_PROC_DIRNAME, NULL);
	unregister_chrdev(fpga_major, DRIVER_NAME);
}

module_init(armadeus_fpga_init);
module_exit(armadeus_fpga_cleanup);

MODULE_AUTHOR("Julien Boibessot / Nicolas Colombain");
MODULE_DESCRIPTION("Armadeus FPGA loading driver");
MODULE_LICENSE("GPL");
