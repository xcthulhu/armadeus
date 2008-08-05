/* 
 ***********************************************************************
 * fpgaaccess.c 
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 *
 * A simple driver for reading and writing on
 * fpga throught a character file /dev/fpgaaccess
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
 **********************************************************************
 */

#include "fpgaaccess.h"

/*************************************/
/* to read write fpgaaccess register */
/*************************************/
ssize_t fpgaaccess_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp){
  struct fpgaaccess_dev *sdev = fildes->private_data;
  u32 data=0;

  if(*offp >= FPGA_SIZE || ((*offp)%2 != 0)){ /* offset must be pair */
    return 0;
  }

  if(count > 4) /* 32bits max*/
    count = 4; 

  if (count == 2)
    data = ioread16(sdev->fpga_virtual_base_address + *offp);
  else        
    data = ioread32(sdev->fpga_virtual_base_address + *offp);
    
 /* return data for user */
  if(copy_to_user(buff,&data,count)){
    printk(KERN_WARNING "read : copy to user data error\n");
    return -EFAULT;
  }
  return 0;
}

ssize_t fpgaaccess_write(struct file *fildes, const char __user *
                  buff,size_t count, loff_t *offp){
  struct fpgaaccess_dev *sdev = fildes->private_data;
  u32 data=0;

  if(*offp >= FPGA_SIZE || ((*offp)%2 != 0)){ /* offset must be pair */
    return 0;
  }

  if(count > 4) /* 32 bits max)*/
    count = 4; 

  if(copy_from_user(&data,buff,count)){
    printk(KERN_WARNING "write : copy from user error\n");
    return -EFAULT;
  }

  if (count == 2)
    iowrite16(data,sdev->fpga_virtual_base_address + *offp);
  else        
    iowrite32(data,sdev->fpga_virtual_base_address + *offp); 

  return count;
}

int fpgaaccess_open(struct inode *inode, struct file *filp){
  filp->private_data=container_of(inode->i_cdev,struct fpgaaccess_dev,cdev);
  return 0;
}

int fpgaaccess_release(struct inode *inode, struct file *filp){
  filp->private_data=NULL;
  return 0;
}



/********************************/
/* init and release module      */
/********************************/

static int __init fpgaaccess_init(void){
  int result;
  int fpgaaccess_major,fpgaaccess_minor;
  struct fpgaaccess_dev *sdev;

  fpgaaccess_major = 250;
  fpgaaccess_minor = 0;

  /* Allocate a private structure and reference it as driver's data */
  PDEBUG("Allocate a private structure and reference it as driver s data\n"); 
  sdev = (struct fpgaaccess_dev *)kmalloc(sizeof(struct fpgaaccess_dev),GFP_KERNEL);
  if(sdev == NULL){
    printk(KERN_WARNING "FPGAACCESS: unable to allocate private structure\n");
    return -ENOMEM;
  }

  fpgaaccessdev = sdev;

  /* Get the major and minor device numbers */
  PDEBUG("Get the major and minor device numbers\n");
  if(fpgaaccess_major){
    devno = MKDEV(fpgaaccess_major,fpgaaccess_minor);
    result = register_chrdev_region(devno, N_DEV,FPGAACCESS_NAME);
  }else{
    result = alloc_chrdev_region(&devno,fpgaaccess_minor,N_DEV,FPGAACCESS_NAME);
    fpgaaccess_major = MAJOR(devno);
  }
  printk(KERN_INFO "fpgaaccess: MAJOR: %d MINOR: %d\n",MAJOR(devno),MINOR(devno));
  if(result < 0){
    printk(KERN_ERR "fpgaaccess: can't get major %d\n",fpgaaccess_major);
    goto mkdev_error;
  }

  /* Init the cdev structure  */
  PDEBUG("Init the cdev structure\n");
  cdev_init(&sdev->cdev,&fpgaaccess_fops);
  sdev->cdev.owner = THIS_MODULE;

  /* Add the device to the kernel, connecting cdev to major/minor number */
  PDEBUG("Add the device to the kernel, connecting cdev to major/minor number \n");
  result = cdev_add(&sdev->cdev,devno,1);
  if(result < 0){
    printk(KERN_WARNING "FPGAACCESS: can't add cdev\n");
    goto cdevadd_error;
  }

  sdev->fpga_virtual_base_address = (void*)FPGA_BASE_ADDR;

  /* ok module insered */
  fpgaaccessdev=sdev;
  printk(KERN_INFO "FPGAACCESS module insered\n");
  return 0;

cdevadd_error:
  /* free major and minor number */
  unregister_chrdev_region(devno,N_DEV);
  printk(KERN_INFO "fpgaaccess module deleted\n");
mkdev_error:
  printk(KERN_ERR "%s: not insered\n",FPGAACCESS_NAME);
  kfree(sdev);
  return result;

}

static void __exit fpgaaccess_exit(void){
  struct fpgaaccess_dev *sdev;

  sdev = fpgaaccessdev;

  /* delete the cdev structure */
  cdev_del(&sdev->cdev);

  /* free major and minor number */
  unregister_chrdev_region(devno,N_DEV);
  printk(KERN_INFO "fpgaaccess module deleted\n");

  /* freed structure device */
  kfree(sdev);
}


