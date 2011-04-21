/*
 ***********************************************************************
 *
 * (c) Copyright 2007    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * Driver for Wb_button IP
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


#ifndef __BUTTON_H__
#define __BUTTON_H__

#define BUTTON_NUMBER 1

#define BUTTON_REG_OFFSET (0x02)
#define BUTTON_ID_OFFSET  (0x00)

/* platform device */
struct plat_button_port {
	const char *name;	/*instance name */
	int interrupt_number;	/* interrupt_number */
	int num;		/* instance number */
	void *membase;		/* ioremap base address */
	int idnum;		/* identity number */
	int idoffset;		/* identity relative address */
	struct button_dev *sdev;/* struct for main device structure*/
};

#endif /* __BUTTON_H__ */
