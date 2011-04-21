/********************************************************************
 * NXP ISP176x Debugging and Conditinal Compilation header file
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
 * File Name: isp1761.h
 *
 * Refering linux kernel version 2.6.9
 *
 * History:
 *
 * Date               Author                  Comments
 * --------------------------------------------------------------------
 * Nov 29 2005       Prabhakar Kalasani       Initial version
 * May 5, 2006       Grant Hatamosa           Added ISO support
 * May 5, 2006       Grace Baldonasa          Added MTP support (Available only
 *                                            with Peripheral Controller code)
 *
 **********************************************************************
 */


/*******************START HOST CONTROLLER********************************/
/* Host controller conditional defines*/
#define MSEC_INT_BASED                                  /* define to run host on SOF interrupt */

/* For Iso support */
#ifdef MSEC_INT_BASED /* Iso support is only currently available in this mode */
#define CONFIG_ISO_SUPPORT /* Comment to remove isochronous transfer support */
#endif

#ifdef CONFIG_ISO_SUPPORT

#define ISO_DBG_ENTRY FALSE
#define ISO_DBG_EXIT FALSE
#define ISO_DBG_ADDR TRUE
#define ISO_DBG_DATA TRUE
#define ISO_DBG_ERR  TRUE
#define ISO_DBG_INFO TRUE

#if 0 /* Set to 1 to enable isochronous debugging */
#define iso_dbg(category, format, arg...) \
do \
{ \
    if(category) \
    { \
        printk(format, ## arg); \
    } \
} while(0)
#else
#define iso_dbg(category, format, arg...) while(0)
#endif

#endif /* CONFIG_ISO_SUPPORT */

		/*End ISO support stuff*/
	/*------------------------------------------*/

/*Debug For Entry/Exit of the functions */ 
#undef HCD_DEBUG_LEVEL1
#ifdef HCD_DEBUG_LEVEL1
#define pehci_entry(format, args... ) printk(format, ##args)
#else
#define pehci_entry(format, args...) do { } while(0)
#endif

/*Debug for Port Info and Errors */
#undef HCD_DEBUG_LEVEL2
#ifdef HCD_DEBUG_LEVEL2
#define pehci_print(format, args... ) printk(format, ##args)
#else
#define pehci_print(format, args...) do { } while(0)
#endif

/*Debug For the Port changes and Enumeration */
#undef HCD_DEBUG_LEVEL3
#ifdef HCD_DEBUG_LEVEL3
#define pehci_info(format,arg...) printk(format, ##arg)
#else
#define pehci_info(format,arg...) do {} while (0)
#endif

/*Debug For Transfer flow  */
#undef HCD_DEBUG_LEVEL4
#ifdef HCD_DEBUG_LEVEL4
#define pehci_check(format,args...) printk(format, ##args)
#else
#define pehci_check(format,args...)
#endif 
/*******************END HOST CONTROLLER**********************************/



/*******************START DEVICE CONTROLLER******************************/

/* For MTP support */
#undef MTP_ENABLE /* Enable to add MTP support on ISP1761 Device Controller
                   * Requires MTP class driver 
                   */

/*Debug Entery/Exit of Function as well as some other Information(I'm not sure)*/
#undef DEV_DEBUG_LEVEL2
#ifdef DEV_DEBUG_LEVEL2
#define dev_print(format,arg...) printk(format, ##arg)
#else
#define dev_print(format,arg...) do {} while (0)
#endif

/*Debug for Interrupt , Registers , Device Enable/Disable and some other info */
#undef DEV_DEBUG_LEVEL3
#ifdef DEV_DEBUG_LEVEL3
#define dev_inform(format,arg...) printk(format, ##arg)
#else
#define dev_inform(format,arg...) do {} while (0)
#endif

/*Debug for Transfer flow , Enumeration and Packet info */
#undef DEV_DEBUG_LEVEL4
#ifdef DEV_DEBUG_LEVEL4
#define dev_check(format,args...) printk(format, ##args)
#else
#define dev_check(format,args...) do{}while(0)
#endif
/*******************END DEVICE CONTROLLER********************************/


/*******************START MSCD*******************************************/
/*Debug Entery/Exit of Function as well as some other Information(I'm not sure)*/
#undef MSCD_DEBUG_LEVEL2
#ifdef MSCD_DEBUG_LEVEL2
#define mscd_print(format,arg...) printk(format, ##arg)
#else
#define mscd_print(format,arg...) do {} while (0)
#endif

/*Debug for Info */
#undef MSCD_DEBUG_LEVEL3
#ifdef MSCD_DEBUG_LEVEL3
#define mscd_info(format,arg...) printk(format, ##arg)
#else
#define mscd_info(format,arg...) do {} while (0)
#endif
/*******************END MSCD*********************************************/


/*******************START OTG CONTROLLER*********************************/
#undef OTG      /*undef for Host only and Device only */
#undef ALL_FSM_FLAGS
/*Debug for Entry/Exit and Info */
#undef OTG_DEBUG_LEVEL1
#ifdef OTG_DEBUG_LEVEL1
#define otg_entry(format, args... ) printk(format, ##args)
#else
#define otg_entry(format, args...) do { } while(0)
#endif

/*Debug for State Machine Flow */
#undef OTG_DEBUG_LEVEL2
#ifdef OTG_DEBUG_LEVEL2
#define otg_print(format,arg...) printk(format, ##arg)
#else
#define otg_print(format,arg...) do {} while (0)
#endif
/*Debug for Info */
#undef OTG_DEBUG_LEVEL3
#ifdef OTG_DEBUG_LEVEL3
#define otg_info(format,arg...) printk(format, ##arg)
#else
#define otg_info(format,arg...) do {} while (0)
#endif
/*******************END OTG CONTROLLER***********************************/



/*******************START FOR HAL ***************************************/
/*Debug For Entry and Exit of the functions */ 
#undef HAL_DEBUG_LEVEL1
#ifdef HAL_DEBUG_LEVEL1
#define hal_entry(format, args... ) printk(format, ##args)
#else
#define hal_entry(format, args...) do { } while(0)
#endif

/*Debug For Interrupt information */ 
#undef HAL_DEBUG_LEVEL2
#ifdef HAL_DEBUG_LEVEL2
#define hal_int(format, args... ) printk(format, ##args)
#else
#define hal_int(format, args...) do { } while(0)
#endif

/*Debug For HAL Initialisation and Mem Initialisation */ 
#undef HAL_DEBUG_LEVEL3
#ifdef HAL_DEBUG_LEVEL3
#define hal_init(format, args... ) printk(format, ##args)
#else
#define hal_init(format, args...) do { } while(0)
#endif
/*******************END FOR HAL*******************************************/



/*******************START FOR ALL CONTROLLERS*****************************/
#undef CONFIG_USB_OTG   /*undef for Host only and Device only */
#define ISP1761_DEVICE 

#ifdef CONFIG_USB_DEBUG
#define DEBUG
#else
#undef DEBUG
#endif
/*******************END FOR ALL CONTROLLERS*******************************/

#include <linux/version.h>
#define info(format, arg...) printk(KERN_INFO KBUILD_MODNAME ": " format "\n" , ## arg)
#define warn(format, arg...) printk(KERN_WARNING KBUILD_MODNAME ": " format "\n" , ## arg)

