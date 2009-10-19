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
**
*/

#ifndef __ASMAX5821_HPP__
#define __ASMAX5821_HPP__

#include <stdio.h>
#include <stdlib.h>
#include "as_dac.hpp"
#include "as_i2c.hpp"

using namespace std;

/**
 *
 */
class AsMax5821 : public AsDac{

public:

        /* Singleton constructor */
        static AsMax5821 *getInstance(int aI2cNum);
        static AsMax5821 *getInstance(int aI2cNum, int aI2cChipAddress);

        /* destructor */
        virtual ~AsMax5821();

        virtual AsDac_errors setDac(void * aObject, int aValue);
        virtual AsDac_errors setDac(char aChannel, int aValue);
        virtual AsDac_errors setDacPercent(void * aObject, float aValueInPercent);
        virtual AsDac_errors setDacPercent(char aChannel, float aValueInPercent);

        /* */
        virtual AsDac_errors powerDown(void * aObject);
        virtual AsDac_errors powerDown(char aChannel);

        static const int MAX5821L_IC_SLAVE_ADDRESS_GND      = 0x38; 
        static const int MAX5821L_IC_SLAVE_ADDRESS_VDD      = 0x39;
        static const int MAX5821M_IC_SLAVE_ADDRESS_GND      = 0x58;
        static const int MAX5821M_IC_SLAVE_ADDRESS_VDD      = 0x59;

private:
        AsI2c * mI2c;
        unsigned char mI2cNum;
        unsigned char mI2cChipAddress;

        /* Constants */
        static const int NUMBER_OF_CHANNEL = 2 ; /**< Number of channels in component */
        static const int MAX5821_LOAD_DAC_A_IN_REG_B        = 0x0; 
        static const int MAX5821_LOAD_DAC_B_IN_REG_A        = 0x1;
        static const int MAX5821_LOAD_DAC_A                 = 0x4;
        static const int MAX5821_LOAD_DAC_B                 = 0x5;
        static const int MAX5821_LOAD_DAC_A_IN_UPDATE_ALL   = 0x8;
        static const int MAX5821_LOAD_DAC_B_IN_UPDATE_ALL   = 0x9;
        static const int MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL = 0xC;
        static const int MAX5821_LOAD_DAC_ALL_IN            = 0xD;

        static const int MAX5821_UPDATE_ALL_DAC_COMMAND     = 0xE;
        static const int MAX5821_EXTENDED_COMMAND_MODE      = 0xF;
        static const int MAX5821_READ_DAC_A_COMMAND         = 0x1;
        static const int MAX5821_READ_DAC_B_COMMAND         = 0x2;

        static const int POWER_UP                           = 0x0;
        static const int POWER_DOWN_MODE0                   = 0x1;
        static const int POWER_DOWN_MODE1                   = 0x2;
        static const int POWER_DOWN_MODE2                   = 0x3;

        static const int POWER_CTRL_SELECTED                = 0x1;
        static const int POWER_CTRL_UNSELECTED              = 0x0;

        static const int MAX5821M_MAX_DATA_VALUE            = 1024;

        static const char * I2C_URL;

        /* transfered data structs. */
        typedef struct 
        {
            unsigned char command;
            unsigned char ctrlB;
            unsigned char ctrlA;
            unsigned char powerMode;
        }  extended_command;
        
        typedef struct 
        {
            unsigned char  command;
            unsigned short data;
            unsigned char  Sbits;
        } data_command;
        
        /* Singleton */
        static AsMax5821 *mMax5821;

        AsMax5821(int aI2cNum, int aI2cChipAddress); /* Private constructor */
        virtual AsDac_errors init(int aI2cNum);

        /* Global values */
        int mI2cHandler;
        AsDac_errors build_data_command(data_command aInSruct, unsigned char * aBuf );
        AsDac_errors build_extended_command(extended_command aInSruct, unsigned char * aBuf );

};

#endif /* __ASMAX5821_HPP__ */


