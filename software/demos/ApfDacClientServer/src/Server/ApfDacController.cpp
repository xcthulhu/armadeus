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
**	File name: ApfDacController.cpp
**
**	author: carbure@users.sourceforge.net
*/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// #include <linux/i2c.h> // seems to make trouble during compiling ....
#include <linux/i2c-dev.h>
 
#include <sys/ioctl.h>

#include "ApfDacController.h"

#define Build_data_command( InSruct, Buf ) \
{ \
	Buf[0] = (InSruct.command << 4) + (unsigned  char) (InSruct.data>>6);\
	Buf[1] = (unsigned  char) (InSruct.data<<2);\
} 

#define Build_extended_command( InSruct, Buf ) \
{ \
	Buf[0] = (InSruct.command << 4) ;\
	Buf[1] = (InSruct.ctrlB << 3) +  (InSruct.ctrlA << 2) + InSruct.powerMode;\
} 


//******************************************************************************

ApfDacController::ApfDacController(int minLeft, int maxLeft ,int minRight, int maxRight) :
mMinLeft(minLeft), mMaxLeft(maxLeft) , mMinRight(minRight), mMaxRight(maxRight),mOpen(false)
{
	if ((mI2cBus = open("/dev/i2c-0",O_RDWR)) >= 0)
	{
		if ( ioctl(mI2cBus ,/*I2C_SLAVE*/ 0x703, MAX5821L_IC_SLAVE_ADDRESS_GND) >= 0)
		{
			mOpen =true;
		}
		mPowerDownCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
		mPowerDownCommand.powerMode = POWER_DOWN_MODE0 ;
		mPowerUpCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
		mPowerUpCommand.powerMode = POWER_UP ;	
		mDACOutput.Sbits = 0;
	}

}

//******************************************************************************

ApfDacController::~ApfDacController()
{
}

//******************************************************************************

void ApfDacController::setLeftDAC(int value)
{
	unsigned char buf[2];

	if ( (mOpen==true) && (value>= mMinLeft) && (value <= mMaxLeft) )
	{
		value = (value * MAX5821M_MAX_DATA_VALUE) / (mMaxLeft-mMinLeft);
//		printf("Set the left DAC : %d\n", value);

		mPowerUpCommand.ctrlA = POWER_CTRL_SELECTED;
		mPowerUpCommand.ctrlB = POWER_CTRL_UNSELECTED;
		mDACOutput.command  =  MAX5821_LOAD_DAC_A_IN_REG_B ;
		mDACOutput.data = value;
		Build_extended_command( mPowerUpCommand, buf);
		write (mI2cBus, buf,2);
		Build_data_command(mDACOutput,buf);
		write (mI2cBus, buf,2);
	}
}

//******************************************************************************

void ApfDacController::setRightDAC(int value)
{
	unsigned char buf[2];

	if ( (mOpen==true) && (value>= mMinRight) && (value <= mMaxRight) )
	{
		value = (value * MAX5821M_MAX_DATA_VALUE) / (mMaxLeft-mMinLeft);
//		printf("Set the right DAC : %d\n", value);
		mPowerUpCommand.ctrlA = POWER_CTRL_UNSELECTED;
		mPowerUpCommand.ctrlB = POWER_CTRL_SELECTED;
		mDACOutput.command  =  MAX5821_LOAD_DAC_B_IN_REG_A;
		mDACOutput.data = value;
		Build_extended_command( mPowerUpCommand, buf);
		write (mI2cBus, buf,2);
		Build_data_command(mDACOutput,buf);
		write (mI2cBus, buf,2);
	}
}

//******************************************************************************

void ApfDacController::setbothDAC(int value)
{
	unsigned char buf[2];

	if ( (mOpen==true) && (value>= mMinRight) && (value <= mMaxRight) && (value>= mMinLeft) && (value <= mMaxLeft))
	{
		value = (value * MAX5821M_MAX_DATA_VALUE) / (mMaxLeft-mMinLeft);
		mPowerDownCommand.ctrlA = POWER_CTRL_SELECTED;
		mPowerDownCommand.ctrlB = POWER_CTRL_SELECTED;
		mPowerUpCommand.ctrlA = POWER_CTRL_SELECTED;
		mPowerUpCommand.ctrlB = POWER_CTRL_SELECTED;
		mDACOutput.command  =  MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL ;
		mDACOutput.data = value;
		Build_extended_command( mPowerUpCommand, buf);
		write (mI2cBus, buf,2);
		Build_data_command(mDACOutput,buf);
		write (mI2cBus, buf,2);
	}
}

//******************************************************************************

void ApfDacController::powerDown()
{
	unsigned char buf[2];

	mPowerDownCommand.ctrlA = POWER_CTRL_SELECTED;
	mPowerDownCommand.ctrlB = POWER_CTRL_SELECTED;
	Build_extended_command( mPowerDownCommand, buf);
	write (mI2cBus, buf,2);
}

//******************************************************************************

bool ApfDacController::isOpen()
{
	return mOpen;
}

//******************************************************************************


