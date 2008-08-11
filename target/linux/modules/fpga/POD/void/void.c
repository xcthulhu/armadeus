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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

//
// Module's initialization function
//
static int __init void_init(void)
{
    printk("Init void module\n");
       
    return(0);
}

//
// Module's cleanup function
//
static void __exit void_exit(void)
{
    printk("Leave void module\n");
}

module_init(void_init);
module_exit(void_exit);
MODULE_AUTHOR("Fabien Marteau");
MODULE_DESCRIPTION("Void driver");
MODULE_LICENSE("GPL");
