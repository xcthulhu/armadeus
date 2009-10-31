/*
**	THE ARMADEUS PROJECT
**
**  Copyright (C) year  The source forge armadeus project team
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
**	File : max5821.h 
**
**	author: carbure@users.sourceforge.net
*/ 


/* I2C slave addresses */
#define	MAX5821L_IC_SLAVE_ADDRESS_GND		0x38
#define	MAX5821L_IC_SLAVE_ADDRESS_VDD		0x39
#define	MAX5821M_IC_SLAVE_ADDRESS_GND		0x58
#define	MAX5821M_IC_SLAVE_ADDRESS_VDD		0x59

/* command bytes */
#define	MAX5821_LOAD_DAC_A_IN_REG_B		0x0
#define	MAX5821_LOAD_DAC_B_IN_REG_A		0x1
#define	MAX5821_LOAD_DAC_A			0x4
#define	MAX5821_LOAD_DAC_B			0x5
#define	MAX5821_LOAD_DAC_A_IN_UPDATE_ALL	0x8
#define	MAX5821_LOAD_DAC_B_IN_UPDATE_ALL	0x9
#define	MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL	0xC
#define	MAX5821_LOAD_DAC_ALL_IN			0xD

#define	MAX5821_UPDATE_ALL_DAC_COMMAND		0xE
#define	MAX5821_EXTENDED_COMMAND_MODE		0xF
#define	MAX5821_READ_DAC_A_COMMAND		0x1
#define	MAX5821_READ_DAC_B_COMMAND		0x2

#define	POWER_UP				0x0
#define	POWER_DOWN_MODE0			0x1
#define	POWER_DOWN_MODE1			0x2
#define	POWER_DOWN_MODE2			0x3

#define POWER_CTRL_SELECTED			0x1
#define POWER_CTRL_UNSELECTED			0x0

#define	MAX5821M_MAX_DATA_VALUE			1023

/* transfered data structs */
typedef	struct 
{
	unsigned char	command;
	unsigned char	ctrlB;
	unsigned char	ctrlA;
	unsigned char	powerMode;
} extended_command;

typedef	struct 
{
	unsigned char	command;
	unsigned short	data;
	unsigned char	Sbits;
} data_command;

typedef	struct 
{
	unsigned char	outputSelect;
	unsigned char	command;
} read_command;

typedef	struct 
{
	unsigned short	powerMode;
	unsigned short	data;
	unsigned short	Sbits;
} data_read;

#define BUILD_DATA_COMMAND(InSruct, Buf) \
{ \
	Buf[0] = (InSruct.command << 4) + (unsigned char) (InSruct.data >> 6);\
	Buf[1] = (unsigned char) (InSruct.data << 2);\
} 

#define BUILD_EXTENDED_COMMAND(InSruct, Buf) \
{ \
	Buf[0] = (InSruct.command << 4) ;\
	Buf[1] = (InSruct.ctrlB << 3) +  (InSruct.ctrlA << 2) + InSruct.powerMode;\
}
