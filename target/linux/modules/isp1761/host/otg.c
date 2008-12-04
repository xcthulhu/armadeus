/********************************************************************
 * NXP ISP176x Host Controller Interface code file
 *
 * (c) 2006 NXP B.V., All rights reserved. <usb.linux@nxp.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * File Name: otg.c
 *
 * Refering linux kernel version 2.6.9
 *
 * History:
 *
 * Date                  Author                  Comments
 * --------------------------------------------------------------------
 * June 21, 2006          krishan                 Initial version
 * 
 **********************************************************************
 */

#ifdef OTG

/*otg device*/
struct usb_device *otgdev = 0;
/*otg hub urb*/
struct urb    *otgurb;

#define OTG_DEVICE_ADDRESS  (2)
#define OTG_PORT_NUMBER     (0)

/*return otghub from here*/
struct usb_device *phci_register_otg_device (struct isp1761_dev *dev)
{
    if(otgdev && otgdev->devnum == 0x2)
        return otgdev;

    return NULL;        
}

/*suspend the otg port(0)
 * needed when port is switching
 * from host to device
 * */
int
phci_suspend_otg_port(struct isp1761_dev *dev,
        u32   command)
{
    int status = 0;
    otgdev->otgstate = USB_OTG_SUSPEND;
    if(otgurb->status == -EINPROGRESS)
        otgurb->status = 0;

    /*complete the urb*/
    otgurb->complete(otgurb,NULL);      

    /*reset the otghub urb status*/
    otgurb->status = -EINPROGRESS;
    g_1761_enum_complete=0;
    return status;
}

/*set the flag to enumerate the device*/
int
phci_enumerate_otg_port(struct isp1761_dev *dev,
        u32 command)
{
    /*set the flag to enumerate*/
    /*connect change interrupt will happen from
     * phci_intl_worker only
     * */
    otgdev->otgstate = USB_OTG_ENUMERATE;
    if(otgurb->status == -EINPROGRESS)
        otgurb->status = 0;
    /*complete the urb*/
    otgurb->complete(otgurb,NULL);
    /*reset the otghub urb status*/
    otgurb->status = -EINPROGRESS;
    while(1)
    {
        if(g_1761_enum_complete == 1)
            break;
    }
    g_1761_enum_complete=0;
    return 0;
}

/*host controller resume sequence at otg port*/
int
phci_resume_otg_port(struct isp1761_dev *dev,
        u32   command)
{
    printk("Resume is called\n");
    otgdev->otgstate = USB_OTG_RESUME;
    if(otgurb->status == -EINPROGRESS)
        otgurb->status = 0;
    /*complete the urb*/
    otgurb->complete(otgurb,NULL);
    /*reset the otghub urb status*/
    otgurb->status = -EINPROGRESS;
    return 0;
}
EXPORT_SYMBOL(phci_register_otg_device);
EXPORT_SYMBOL(phci_enumerate_otg_port);
EXPORT_SYMBOL(phci_suspend_otg_port);
EXPORT_SYMBOL(phci_resume_otg_port);
#endif

