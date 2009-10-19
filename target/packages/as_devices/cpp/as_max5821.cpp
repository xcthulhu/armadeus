/*
**    THE ARMADEUS PROJECT
** 
**    Copyright (C) 2009  The armadeus systems team 
**    Fabien Marteau <fabien.marteau@armadeus.com>
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "as_max5821.hpp"

const char * AsMax5821::I2C_URL = "/dev/i2c-";

AsMax5821 * AsMax5821::mMax5821 = NULL;

/*------------------------------------------------------------------------------*/

/** Singleton constructor
 * @param aI2cNum i2c bus number
 * @return AsMax5821 class
 */
AsMax5821*
AsMax5821::getInstance(int aI2cNum)
{
    return getInstance(aI2cNum,MAX5821L_IC_SLAVE_ADDRESS_GND);
}
/*------------------------------------------------------------------------------*/

/** Singleton constructor
 * @param aI2cNum i2c bus number
 * @param aI2cChipAddress i2c chip slave address
 * @return AsMax5821 class
 */
AsMax5821*
AsMax5821::getInstance(int aI2cNum, int aI2cChipAddress)
{
    if(mMax5821 == NULL)
    {
        mMax5821 = new AsMax5821(aI2cNum,aI2cChipAddress);
        mMax5821->init(aI2cNum);
    }
    return mMax5821;
}


/*------------------------------------------------------------------------------*/

AsMax5821::AsMax5821(int aI2cNum, int aI2cChipAddress)
{
    mChannelNumber = NUMBER_OF_CHANNEL;
    mI2cNum = aI2cNum;
    mI2cChipAddress = aI2cChipAddress;
    mChannelObject = (void **)malloc(sizeof(void *)*NUMBER_OF_CHANNEL);
    for(int i=0; i < NUMBER_OF_CHANNEL; i++)
    {
        mChannelObject[i] = NULL;
    }
}

/*------------------------------------------------------------------------------*/

AsMax5821::~AsMax5821()
{
    free(mChannelObject);
    
    mMax5821 = NULL;
}

/*------------------------------------------------------------------------------*/

/** Initialize the dac
 *
 * @param 
 * @return
 */
AsDac::AsDac_errors
AsMax5821::init(int aI2cNum)
{
    mI2c = AsI2c::getInstance(mI2cNum);
    return AS_DAC_OK;    
}
/*------------------------------------------------------------------------------*/

/** Set DAC with absolute value (0..1024)
 *
 * @param aChannel channel 'A' or 'B'
 * @param aValue value
 */
AsDac::AsDac_errors
AsMax5821::setDac(char aChannel, int aValue)
{
    extended_command powerUpCommand;
    data_command     setDACOutput;
    unsigned char buf[2];
    AsDac_errors ret;

    powerUpCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
    powerUpCommand.powerMode = POWER_UP ;
    if (aChannel == 'A')
    {
        powerUpCommand.ctrlA = POWER_CTRL_SELECTED;
        powerUpCommand.ctrlB = POWER_CTRL_UNSELECTED;
		setDACOutput.command  =  MAX5821_LOAD_DAC_A_IN_REG_B ;
    }else if(aChannel == 'B')
    {
        powerUpCommand.ctrlA = POWER_CTRL_UNSELECTED;
        powerUpCommand.ctrlB = POWER_CTRL_SELECTED;
	    setDACOutput.command  =  MAX5821_LOAD_DAC_B_IN_REG_A ;
    }else{
        powerUpCommand.ctrlA = POWER_CTRL_SELECTED;
        powerUpCommand.ctrlB = POWER_CTRL_SELECTED;
		setDACOutput.command  =  MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL ;
    }

    // first, we  power up the DAC
    ret = build_extended_command( powerUpCommand, buf);
    if (ret != AS_DAC_OK)
    {
        return ret;
    }
    if ( 0 > mI2c->writeMultipleBytes(mI2cChipAddress, buf,2))
    {
        return AS_DAC_WRITEFILE;
    }
    setDACOutput.data = aValue;
    ret = build_data_command(setDACOutput,buf);
    if (ret != AS_DAC_OK)
    {
        return ret;
    }
    if ( 0 > mI2c->writeMultipleBytes(mI2cChipAddress, buf,2))
    {
        return AS_DAC_WRITEFILE;
    }

  return AS_DAC_OK;
}


/*------------------------------------------------------------------------------*/

/** Set DAC with absolute value (0..1024)
 *
 * @param aChannel channel 'A' or 'B'
 * @param aValue value
 */
AsDac::AsDac_errors
AsMax5821::setDac(void * aObject, int aValue)
{
    char channel;

    channel = getObjectChannel(aObject);
    return setDac(channel,aValue);
}

/*------------------------------------------------------------------------------*/

/** Initialize the DAC with percent value (0..99%)
 *
 * @param aChannel channel 'A' or 'B'
 * @param aValueInPercent value in percent 
 */
AsDac::AsDac_errors
AsMax5821::setDacPercent(void * aObject, float aValueInPercent)
{
    return setDacPercent(getObjectChannel(aObject),aValueInPercent);
}

/*------------------------------------------------------------------------------*/

/** Initialize the DAC with percent value (0..99%)
 *
 * @param aChannel channel 'A' or 'B'
 * @param aValueInPercent value in percent 
 */
AsDac::AsDac_errors
AsMax5821::setDacPercent(char aChannel, float aValueInPercent)
{
    return setDac(aChannel,(int)(aValueInPercent*MAX5821M_MAX_DATA_VALUE/100));
}

/*------------------------------------------------------------------------------*/

/** Power down the DAC
 *
 * @return errors
 */
AsDac::AsDac_errors
AsMax5821::powerDown(char aChannel)
{
    extended_command powerDownCommand;
    unsigned char buf[2];
    AsDac_errors ret;

    powerDownCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
    powerDownCommand.powerMode = POWER_DOWN_MODE0 ;

	powerDownCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
	powerDownCommand.powerMode = POWER_DOWN_MODE0 ;

    if (aChannel == 'A')
    {
		powerDownCommand.ctrlA = POWER_CTRL_SELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_UNSELECTED;
    }else if(aChannel == 'B') {
		powerDownCommand.ctrlA = POWER_CTRL_UNSELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_SELECTED;
    }else{
		powerDownCommand.ctrlA = POWER_CTRL_SELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_SELECTED;
    }

    // first, we  power up the DAC
    ret = build_extended_command( powerDownCommand, buf);
    if (ret != AS_DAC_OK)
    {
        return ret;
    }
    if ( 0 > mI2c->writeMultipleBytes(mI2cChipAddress, buf, 2))
    {
        printf("Write error\n");
        exit (1);
    }
    return AS_DAC_OK;
}


/*------------------------------------------------------------------------------*/

/** Power down the DAC
 *
 * @return errors
 */
AsDac::AsDac_errors
AsMax5821::powerDown(void * aObject)
{
    return powerDown(getObjectChannel(aObject));
}

/*------------------------------------------------------------------------------*/

/** Build data command
 *
 * @param data command structure
 * @param buffer
 * @return error
 */
AsDac::AsDac_errors
AsMax5821::build_data_command(data_command aInSruct, unsigned char * aBuf )
{
    aBuf[0] = (aInSruct.command << 4) + (unsigned  char) (aInSruct.data>>6);
    aBuf[1] = (unsigned  char) (aInSruct.data<<2);
    return AS_DAC_OK;
} 
/*------------------------------------------------------------------------------*/

/** Build extended command
 *
 * @param extended command structure
 * @param buffer
 * @return error
 */
AsDac::AsDac_errors
AsMax5821::build_extended_command(extended_command aInSruct, unsigned char * aBuf )
{
    aBuf[0] = (aInSruct.command << 4) ;
    aBuf[1] = (aInSruct.ctrlB << 3) +  (aInSruct.ctrlA << 2) +\
                aInSruct.powerMode;
    return AS_DAC_OK;
} 

