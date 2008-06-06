/*
 ***********************************************************************
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Driver for Wb_led fpga IP
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

#include "led.h"

/*********************************
 * simple fpga read/write
 *********************************/

static ssize_t led_fpga_read(void * addr,u16 *data,struct led_dev *dev){
  *data = ioread16(addr);/* read the led value */
  //  PDEBUG( "read : value read : %x\n",*data);
  return 2; 
}

static ssize_t led_fpga_write(void * addr,u16 *data,struct led_dev *dev){
  ssize_t retval;

  iowrite16(*data,addr);/* write the led value */
  retval = 2;
  return retval;
}

/* *********************************
 * Sysfs operations
 * *********************************/

static void kobj_release(struct kobject *kobj){
  PDEBUG("led kobject released\n");
}

static int init_sysinterface(struct led_dev *ldev){
  int ret = 0;

  /* initializing kobject to 0 */
  memset(&ldev->kobj,0,sizeof(struct kobject));
  /* set kobject name */
  if(unlikely(ret = kobject_set_name(&ldev->kobj,LED_NAME)))
    goto out;
  /* register */
  if(unlikely(ret=kobject_register(&ldev->kobj)))
    goto out;
  /* register the kobj_type */
  ldev->kobj.ktype = &kobjtype;
  /* add status attribute */
  if(unlikely(ret=sysfs_create_file(&ldev->kobj,&led_status_attr)));

out:
  return ret;
}

static ssize_t led_show_status(struct kobject *kobj,struct attribute *attr,char *buf){
  ssize_t ret_size = 0;
  struct led_dev *dev;
  u16 data;

  dev = container_of(kobj,struct led_dev,kobj);

  if(down_interruptible(&dev->sem)){ /* take mutex */
    printk(KERN_WARNING "led_show_status:can't take mutex\n");
    return -ERESTARTSYS;
  }

  if((ret_size=led_fpga_read(dev->fpga_virtual_base_address + FPGA_LED,&data,dev))<0)
     goto out;

  PDEBUG("data : %d\n",data);
  if(data&0x0001)ret_size = (ssize_t)sprintf(buf,"1\n");
  else ret_size = (ssize_t)sprintf(buf,"0\n");

out:
  up(&dev->sem);
  return ret_size;
}

static ssize_t led_store_status(struct kobject *kobj,struct attribute *attr, 
                                const char *buff, size_t size){
  struct led_dev *ldev = container_of(kobj,struct led_dev,kobj);
  u16 data;

  if(buff[0]=='1')
    data=0x0001;
  else if(buff[0]=='0')
    data=0x0000;
  else 
    return size;

  if(down_interruptible(&ldev->sem))
    return -ERESTARTSYS;

  size=led_fpga_write(ldev->fpga_virtual_base_address + FPGA_LED,&data,ldev);

  up(&ldev->sem);
  return size ;
}

/***********************************
 * characters file /dev operations
 * *********************************/
ssize_t led_read(struct file *fildes, char __user *buff, size_t count, loff_t *offp){
  struct led_dev *ldev = fildes->private_data;
  u16 data=0;
  ssize_t retval = 0;

  PDEBUG( "read : offset %d, count %d\n",(int)*offp,(int)count);
  if(down_interruptible(&ldev->sem)) /* take mutex */
    return -ERESTARTSYS;
  if(*offp >= 1){                    /* offset must be 0 or 1 */
    goto out;
  }
  if(*offp + count >= 1)                    /* Only one word can be read */
    count = 1 - *offp;

  if((retval = led_fpga_read(ldev->fpga_virtual_base_address + FPGA_LED,&data,ldev))<0)
    goto out;

  /* return data for user */
  if(copy_to_user(buff,&data,2)){
    printk(KERN_WARNING "read : copy to user data error\n");
    retval = -EFAULT;
    goto out;
  }

  *offp = *offp + count;
  retval = 1;

out:
  up(&ldev->sem); /* free mutex */
  PDEBUG("read : Return value %d\n",(int)retval);
  return retval;
}

ssize_t led_write(struct file *fildes, const char __user *buff,size_t count, loff_t *offp){
  struct led_dev *ldev = fildes->private_data;
  u16 data=0;
  ssize_t retval = 0;


  PDEBUG("write : offset %d, count %d\n",(int)*offp,(int)count);
  if(down_interruptible(&ldev->sem))
    return -ERESTARTSYS;
  if(*offp != 0){                    /* offset must be 0 */
    goto out;
  }
  if(count != 1)                    /* Only one word can be read */
    count = 1;

  if(copy_from_user(&data,buff,1)){
    printk(KERN_WARNING "write : copy from user error\n");
    retval = -EFAULT;
    goto out;
  }
  
  if((retval=led_fpga_write(ldev->fpga_virtual_base_address + FPGA_LED,&data,ldev))<0){
     goto out;
  }

out:
  up(&ldev->sem);
  return retval;

}

int led_open(struct inode *inode, struct file *filp){
  /* Allocate and fill any data structure to be put in filp->private_data */
  filp->private_data = container_of(inode->i_cdev,struct led_dev, cdev);
  PDEBUG( "Led opened\n");
  return 0;
}

/* ******************************
 * Init and release functions
 * ******************************/
int led_release(struct inode *inode, struct file *filp){
  PDEBUG( "Led released\n");
  return 0;
}

/**********************************
 * Module management
 **********************************/
static int __init led_init(void){

  int result;                 /* error return */
  int led_major,led_minor;
  u16 data;
  struct led_dev *sdev;

  led_major = 253;
  led_minor = 0;

  /* Allocate a private structure and reference it as driver's data */
  PDEBUG("Allocate a private structure and reference it as driver s data\n"); 
  sdev = (struct led_dev *)kmalloc(sizeof(struct led_dev),GFP_KERNEL);
  if(sdev == NULL){
    printk(KERN_WARNING "LED: unable to allocate private structure\n");
    return -ENOMEM;
  }

  /* initiate mutex for accessing led */
  PDEBUG("initiate mutex for accessing led\n");
  init_MUTEX(&sdev->sem);

  /* Get the major and minor device numbers */
  PDEBUG("Get the major and minor device numbers\n");
  if(led_major){
    devno = MKDEV(led_major,led_minor);
    result = register_chrdev_region(devno, N_DEV,LED_NAME);
  }else{
    result = alloc_chrdev_region(&devno,led_minor,N_DEV,LED_NAME);
    led_major = MAJOR(devno);
  }
  printk(KERN_INFO "LED: MAJOR: %d MINOR: %d\n",MAJOR(devno),MINOR(devno));
  if(result < 0){
    printk(KERN_WARNING "LED: can't get major %d\n",led_major);
  }

  /* Init the cdev structure  */
  PDEBUG("Init the cdev structure\n");
  cdev_init(&sdev->cdev,&led_fops);
  sdev->cdev.owner = THIS_MODULE;

  /* Add the device to the kernel, connecting cdev to major/minor number */
  PDEBUG("Add the device to the kernel, connecting cdev to major/minor number \n");
  result = cdev_add(&sdev->cdev,devno,1);
  if(result < 0)printk(KERN_WARNING "LED: can't add cdev\n");

  sdev->fpga_virtual_base_address = (void*)IMX_CS1_VIRT;

  /* initialyzing sysfs */
  PDEBUG(" initialyzing sysfs\n");
  if(unlikely(result = init_sysinterface(sdev)))
    goto error;

  /* initialize led value */
  data=1;
  if((result=led_fpga_write(sdev->fpga_virtual_base_address + FPGA_LED,&data,sdev))<0)
    goto error;

  sdev->previousint = jiffies;
  /* OK module inserted ! */
  leddev=sdev;
  printk(KERN_INFO "Led module insered\n");
  return 0;

error:
  printk(KERN_ERR "%s: not inserted\n",LED_NAME);
  free_all();
  return result;
}

static void __exit led_exit(void){
  free_all();
}

static void free_all(void){
  struct led_dev *sdev;
  sdev = leddev;
  
  /* unregister kobj */
  kobject_unregister(&sdev->kobj);

  /* delete the cdev structure */
  cdev_del(&sdev->cdev);

  /* Free the allocated memory */
  kfree(sdev);

  /* Release I/O memories */
  release_mem_region(FPGA_BASE_ADDR,FPGA_MEM_SIZE);

  /* free major and minor number */
  unregister_chrdev_region(devno,N_DEV);
  printk(KERN_INFO "Led module deleted\n");
}

module_init(led_init);
module_exit(led_exit);