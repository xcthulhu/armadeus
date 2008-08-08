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

/***********************************
 * characters file /dev operations
 * *********************************/
ssize_t led_read(struct file *fildes, char __user *buff,
                 size_t count, loff_t *offp){
    struct led_dev *sdev = fildes->private_data;
    u16 data=0;
    PDEBUG("Read value\n");
    if(*offp != 0 ){ /* offset must be 0 */
        PDEBUG("offset %d\n",(int)*offp);
        return 0;
    }

    PDEBUG("count %d\n",count);
    if(count > 2){ /* 16bits max*/
        count = 2; 
    }

    data = ioread16(sdev->fpga_virtual_base_address + sdev->membase);
    PDEBUG("Read %d at %x\n",data,sdev->membase);

    /* return data for user */
    if(copy_to_user(buff,&data,count)){
        printk(KERN_WARNING "read : copy to user data error\n");
        return -EFAULT;
    }
    return count;
}

ssize_t led_write(struct file *fildes, const char __user *
                         buff,size_t count, loff_t *offp){
    struct led_dev *sdev = fildes->private_data;
    u16 data=0;

    if(*offp != 0){ /* offset must be 0 */
        PDEBUG("offset %d\n",(int)*offp);
        return 0;
    }

    PDEBUG("count %d\n",count);
    if(count > 2){ /* 16 bits max)*/
        count = 2;
    }

    if(copy_from_user(&data,buff,count)){
        printk(KERN_WARNING "write : copy from user error\n");
        return -EFAULT;
    }
    
    PDEBUG("Write %d at %x\n",data,sdev->membase);
    iowrite16(data,sdev->fpga_virtual_base_address + sdev->membase);

    return count;
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
    struct led_dev *dev;
    dev = container_of(inode->i_cdev,struct led_dev,cdev);
    PDEBUG( "%s: released\n",dev->name);
    filp->private_data=NULL;
    return 0;
}

int register_led(struct plat_led_port *dev){
    int result = 0;                 /* error return */
    int led_major,led_minor;
    u16 data;
    struct led_dev *sdev = &led_device[dev->num];
    PDEBUG("Register %d led\n",dev->num);
    /* major and minor number */
    led_major = 253;
    led_minor = dev->num ;

    sdev->fpga_virtual_base_address = (void*)IMX_CS1_VIRT;
    sdev->membase = dev->membase;

    /* name and address of the instance */
    sdev->name = (char *)kmalloc((1+strlen(dev->name))*sizeof(char),GFP_KERNEL);
    if(sdev->name == NULL){
        printk("Kmalloc name space error\n");
        goto error;
    }
    if(strncpy(sdev->name,dev->name,1+strlen(dev->name)) < 0){
        printk("copy error");
        goto error;
    }

    /* Get the major and minor device numbers */
    PDEBUG("%s:Get the major and minor device numbers\n",dev->name);
    if(led_major){
        sdev->devno = MKDEV(led_major,led_minor);
        result = register_chrdev_region(sdev->devno, 1,dev->name);
    }else{
        result = alloc_chrdev_region(&sdev->devno,led_minor,1,dev->name);
        led_major = MAJOR(sdev->devno);
    }

    printk(KERN_INFO "%s: MAJOR: %d MINOR: %d\n",dev->name,MAJOR(sdev->devno),MINOR(sdev->devno));
    if(result < 0){
        printk(KERN_WARNING "%s: can't get major %d\n",dev->name,led_major);
        goto error;
    }

    /* Init the cdev structure  */
    PDEBUG("%s:Init the cdev structure\n",dev->name);
    cdev_init(&sdev->cdev,&led_fops);
    sdev->cdev.owner = THIS_MODULE;
    sdev->cdev.ops   = &led_fops;

    /* Add the device to the kernel, connecting cdev to major/minor number */
    PDEBUG("%s:Add the device to the kernel, connecting cdev to major/minor number \n",dev->name);
    result = cdev_add(&sdev->cdev,sdev->devno,1);
    if(result){
        printk(KERN_WARNING "%s: can't add cdev\n",dev->name);
        goto error;
    }

    /* initialize led value */
    data=1;
    iowrite16(data,sdev->fpga_virtual_base_address + sdev->membase);
    PDEBUG("Wrote %x at %x\n",data,sdev->membase);

    /* OK module inserted ! */
    printk(KERN_INFO "Led module %s insered\n",dev->name);
    return 0;

error:
    printk(KERN_ERR "%s: not inserted\n",dev->name);
    free_all(dev);
    return result;
}

/**********************************
 * driver probe
 **********************************/
static int led_probe(struct platform_device *dev){

    /* new code for led with platform device */
    struct plat_led_port *p = dev->dev.platform_data;
    int ret,i;
    
    PDEBUG("Led probing\n");
    /* register each led in plat_led_port tab */
    for (i = 0; p->name!=NULL ; p++, i++) {
        PDEBUG("%s:register led number %d\n",p->name,i);
        ret = register_led(p);
        if(ret){
            printk(KERN_WARNING "Unable to register led %s\n",p->name);
        }
    }
    /* end of new code */
    PDEBUG("All devices loaded\n");
    return 0  ;
}

static int led_remove(struct platform_device *dev){
    struct plat_led_port *p = dev->dev.platform_data;
    int i;
    PDEBUG("Removing leds\n");
    for (i = 0; p->name!=NULL ; p++, i++) {
        PDEBUG("Remove led %d\n",i);
        free_all(p);
    }
    return 0; 
}

static int free_all(struct plat_led_port *dev){
    struct led_dev *sdev = &led_device[dev->num];

    PDEBUG("Unregister %s, number %d\n",dev->name,dev->num);

    /* delete the cdev structure */
    cdev_del(&sdev->cdev);
    PDEBUG("%s:cdev deleted\n",dev->name);

    /* free major and minor number */
    unregister_chrdev_region(sdev->devno,1);
    printk(KERN_INFO "%s: Led deleted\n",dev->name);
    return 0;
}

module_init(led_init);
module_exit(led_exit);
