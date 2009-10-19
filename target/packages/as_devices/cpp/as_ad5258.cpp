/*
**    THE ARMadeus Systems
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

#include "as_ad5258.hpp"

AsAd5258::AsAd5258(int aI2cNumber)
{
    mI2c = AsI2c::getInstance(aI2cNumber);
    mI2cAd5258Addr = I2C_AD5258_ADDR;
}

AsAd5258::~AsAd5258()
{
}

/*------------------------------------------------------------------------------*/
/** Set RDAC value 
 * @param aValue int value between 0 and 63
 */
void 
AsAd5258::setRdac(int aValue)
{
    mI2c->writeByteData(mI2cAd5258Addr,0x00,aValue);
}

/*------------------------------------------------------------------------------*/
/** Get RDAC value
 * @return int value between 0 and 63
 */
int  
AsAd5258::getRdac(void)
{
    unsigned char value; 
    mI2c->readByteData(mI2cAd5258Addr,0x00,&value);
    return value;
}

/*------------------------------------------------------------------------------*/
/** Set EEPROM value for RDAC 
 * @param aValue int value
 */
void 
AsAd5258::setEeprom(int aValue)
{
    mI2c->writeByteData(mI2cAd5258Addr,0x02,aValue);
}

/*------------------------------------------------------------------------------*/
/** Get EEPROM value
 * @return int value
 */
int  
AsAd5258::getEeprom(void)
{
    unsigned char value;
    mI2c->readByteData(mI2cAd5258Addr,0x02,&value);
    return value;
}

/*------------------------------------------------------------------------------*/
/** Store RDAC value in EEPROM
 */
void 
AsAd5258::store(void)
{
    mI2c->writeByte(mI2cAd5258Addr,0x0c);
}

/*------------------------------------------------------------------------------*/
/** Restore RDAC value from EEPROM
 */
void 
AsAd5258::restore(void)
{
    mI2c->writeByte(mI2cAd5258Addr,0x0a);
}


