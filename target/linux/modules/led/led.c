/* Driver for bus_led IP
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * 4 march 2008
 */

#include "led.h"

ssize_t led_read(struct file *fildes, char __user *buff, size_t count, loff_t *offp){
  struct led_dev *ldev = fildes->private_data;
  void * virt_addr;
  u16 data=0;
  ssize_t retval = 0;

  PDEBUG( "offset %d, count %d\n",(int)*offp,(int)count);
  if(down_interruptible(&ldev->sem)) /* take mutex */
    return -ERESTARTSYS;
  if(*offp >= 1){                    /* offset must be 0 or 1 */
    goto out;
  }
  if(*offp + count >= 1)                    /* Only one word can be read */
    count = 1 - *offp;

  virt_addr = ioremap(IMX_CS1_PHYS,1);/* translate hardware address to virtual */
  if(virt_addr==NULL){
    printk(KERN_WARNING "ioremap error\n");
    return -EFAULT;
  }
  PDEBUG("Virtual fpga adress : %lx\n",(long int)virt_addr);
  PDEBUG("Real    fpga adress : %lx\n",(long int)IMX_CS1_PHYS);
  //  rmb();                            
  data = ioread16(virt_addr);/* read the led value */
  PDEBUG( "Value read : %x\n",data);

  iounmap(virt_addr);

  /* return data for user */
  if(copy_to_user(buff,&data,2)){
    retval = -EFAULT;
    goto out;
  }

  *offp = *offp + count;
  retval = 1;

out:
  up(&ldev->sem); /* free mutex */
  PDEBUG("Return value %d\n",(int)retval);
  return retval;
}

ssize_t led_write(struct file *fildes, const char __user *buff,size_t count, loff_t *offp){
  struct led_dev *ldev = fildes->private_data;
  void * virt_addr;
  u16 data;
  ssize_t retval = -ENOMEM;


  PDEBUG("offset %d, count %d\n",(int)*offp,(int)count);
  if(down_interruptible(&ldev->sem))
    return -ERESTARTSYS;
  if(*offp != 0){                    /* offset must be 0 */
    goto out;
  }
  if(count != 1)                    /* Only one word can be read */
    count = 1;

  virt_addr = ioremap(IMX_CS1_PHYS,1);/* translate hardware address to virtual */
  if(virt_addr==NULL){
    printk(KERN_WARNING "ioremap error\n");
    return -EFAULT;
  }
  PDEBUG("Virtual fpga adress : %lx\n",(long int)virt_addr);
  PDEBUG( "Real    fpga adress : %lx\n",(long int)IMX_CS1_PHYS);
  //  rmb();
  if(copy_from_user(&data,buff,2)){
    retval = -EFAULT;
    goto out;
  }
  PDEBUG( "user buff value %d\n",(int)data);
  data = data;
  iowrite16(data,virt_addr);/* read the led value */
  iounmap(virt_addr);
  PDEBUG( "Value wrote : %x\n",data);


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

int led_release(struct inode *inode, struct file *filp){
  PDEBUG( "Led released\n");
  return 0;
}

static int __init led_init(void){

  int result;                 /* error return */
  int led_major,led_minor;    

  led_major = 253;
  led_minor = 0;
  mem = NULL;

  /* Allocate a private structure and reference it as driver's data */
  sdev = (struct led_dev *)kmalloc(sizeof(struct led_dev),GFP_KERNEL);
  if(sdev == NULL){
    printk(KERN_WARNING "LED: unable to allocate private structure\n");
    return -ENOMEM;
  }

  /* initiate mutex for accessing led */
  init_MUTEX(&sdev->sem);

  /* Get the major and minor device numbers */
  if(led_major){
    devno = MKDEV(led_major,led_minor);
    result = register_chrdev_region(devno, N_DEV,"led");
  }else{
    result = alloc_chrdev_region(&devno,led_minor,N_DEV,"led");
    led_major = MAJOR(devno);
  }
  printk(KERN_INFO "LED: MAJOR: %d MINOR: %d\n",MAJOR(devno),MINOR(devno));
  if(result < 0){
    printk(KERN_WARNING "LED: can't get major %d\n",led_major);
  }

  /* Init the cdev structure  */
  cdev_init(&sdev->cdev,&led_fops);
  sdev->cdev.owner = THIS_MODULE;

  /* Add the device to the kernel, connecting cdev to major/minor number */
  result = cdev_add(&sdev->cdev,devno,1);
  if(result < 0)printk(KERN_WARNING "LED: can't add cdev\n");


  /* Requested I/O memory */
  mem = request_mem_region(LED_BASE_ADDR,LED_LENGTH,"led");
  if(mem == NULL)
    printk(KERN_ERR "LED: Error memory allocation\n");



  printk(KERN_INFO "Led module insered\n");
  return 0;
}

static void __exit led_exit(void){

  /* delete the cdev structure */
  cdev_del(&sdev->cdev);

  /* Free the allocated memory */
  kfree(sdev);

  /* Release I/O memories */
  release_mem_region(LED_BASE_ADDR,LED_LENGTH);

  /* free major and minor number */
  unregister_chrdev_region(devno,N_DEV);
  printk(KERN_INFO "Led module deleted\n");
}

module_init(led_init);
module_exit(led_exit);
