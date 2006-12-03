/*
**	THE ARMADEUS PROJECT
**
**  Copyright (2006)  The source forge armadeus project team
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
**	File name: ApfDacController.h
**
**	author: carbure@users.sourceforge.net
*/

#ifndef	__APFDACCONTROLLER_H_INCLUDED__
#define	__APFDACCONTROLLER_H_INCLUDED__

class ApfDacController
{
public:

    ApfDacController(int minLeft, int maxLeft ,int minRight, int maxRight);

    virtual ~ApfDacController();
	void setLeftDAC(int value);
	void setRightDAC(int value);
	void setbothDAC(int value);
	void powerDown();
	bool isOpen();

private:


protected:
	// transfered data structs.
	
	typedef	struct 
	{
		unsigned char		command;
		unsigned char		ctrlB;
		unsigned char		ctrlA;
		unsigned	char		powerMode;
	}  extended_command;
	
	typedef	struct 
	{
		unsigned char	command;
		unsigned short data;
		unsigned char	Sbits;
	} data_command;
	
	typedef	struct 
	{
		unsigned char	outputSelect;
		unsigned char	command;
	} read_command;
	
	
	typedef	struct 
	{
		unsigned	short	powerMode;
		unsigned short	data;
		unsigned short	Sbits;
	} data_read;
	// I2C slave Adresses
	
	static const unsigned char	MAX5821L_IC_SLAVE_ADDRESS_GND = 0x38;
	static const unsigned char	MAX5821L_IC_SLAVE_ADDRESS_VDD = 0x39;
	static const unsigned char	MAX5821M_IC_SLAVE_ADDRESS_GND = 0x58;
	static const unsigned char	MAX5821M_IC_SLAVE_ADDRESS_VDD = 0x59;
	
	// command bytes
	
	static const unsigned char	MAX5821_LOAD_DAC_A_IN_REG_B = 0x0;
	static const unsigned char	MAX5821_LOAD_DAC_B_IN_REG_A = 0x1;
	static const unsigned char	MAX5821_LOAD_DAC_A = 0x4;
	static const unsigned char	MAX5821_LOAD_DAC_B = 0x5;
	static const unsigned char	MAX5821_LOAD_DAC_A_IN_UPDATE_ALL = 0x8;
	static const unsigned char	MAX5821_LOAD_DAC_B_IN_UPDATE_ALL = 0x9;
	static const unsigned char	MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL = 0xC;
	static const unsigned char	MAX5821_LOAD_DAC_ALL_IN = 0xD;
	
	static const unsigned char	MAX5821_UPDATE_ALL_DAC_COMMAND = 0xE;
	static const unsigned char	MAX5821_EXTENDED_COMMAND_MODE = 0xF;
	static const unsigned char	MAX5821_READ_DAC_A_COMMAND = 0x1;
	static const unsigned char	MAX5821_READ_DAC_B_COMMAND = 0x2;
	
	static const unsigned char	POWER_UP = 0x0;
	static const unsigned char	POWER_DOWN_MODE0 = 0x1;
	static const unsigned char	POWER_DOWN_MODE1 = 0x2;
	static const unsigned char	POWER_DOWN_MODE2 = 0x3;
	
	static const unsigned char 	POWER_CTRL_SELECTED = 0x1;
	static const unsigned char 	POWER_CTRL_UNSELECTED = 0x0;
	
	static const int	MAX5821M_MAX_DATA_VALUE = 1023;

	int mMinLeft;
	int mMaxLeft;
	int mMinRight;
	int mMaxRight;
	bool mOpen;

	int mI2cBus;
	extended_command	mPowerDownCommand;
	extended_command	mPowerUpCommand;
	data_command		mDACOutput;

};
#endif	// __APFDACCONTROLLER_H_INCLUDED__

