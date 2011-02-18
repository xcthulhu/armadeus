/*
**	ARMADEUS Systems
**
**  Copyright (C) 2011 year  The source forge armadeus systems team
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
**	File : tfp410ctrl.h 
**
**	authors: thom25@users.sourceforge.net
*/ 
 

// I2C slave Adresses

#define TFP410_I2C_SLAVE_ADDR	0x38


// main registers address
#define VEN_ID		0x014C
#define DEV_ID		0x410
#define REV_ID		0x0000


#define VEN_ID_REG	0x00
#define DEV_ID_REG	0x02
#define REV_ID_REG	0x04
#define CTL_1_MODE_REG	0x08
#define CTL_2_MODE_REG	0x09
#define CTL_3_MODE_REG	0x0A
#define CFG_REG		0x0B
#define DE_DLY_REG	0x32
#define DE_CTL_REG	0x33
#define DE_TOP_REG	0x34
#define DE_CNT_REG	0x36
#define DE_LIN_REG	0x38
#define H_RES_REG	0x3A
#define V_RES_REG	0x3C



