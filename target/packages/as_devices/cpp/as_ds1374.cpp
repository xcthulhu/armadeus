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
#include "as_ds1374.hpp"
/*------------------------------------------------------------------------------*/

AsDs1374::AsDs1374(int aI2cBusNum,int aI2cChipAddr)
{
    mAsI2c = AsI2c::getInstance(aI2cBusNum);
    mI2cChipAddr = aI2cChipAddr;
}
/*------------------------------------------------------------------------------*/

AsDs1374::~AsDs1374()
{
}

/*------------------------------------------------------------------------------*/
/* Set counter value
 * @param aValue counter value
 * @return error code
 */
int 
AsDs1374::setCounter(int aValue)
{
    unsigned char buf[4];

#ifdef DEBUG
    printf("-> 0x%08x\n", aValue);
#endif
    // create an I2C write message
    buf[0] = DS1374_CNT_REG;
    buf[1] = aValue & 0xff;
    buf[2] = (aValue >> 8) & 0xff;
    buf[3] = (aValue >> 16) & 0xff;

    return mAsI2c->writeMultipleBytes(mI2cChipAddr,buf,4);
}
/*------------------------------------------------------------------------------*/

/**
*   (De)Activate watchdog.(true/false)
*     Returns: 0 = success, 1 = failure
*/
int 
AsDs1374::activate(bool state)
{
    if (state)
    {   /* WACE + WD + WDSTR + AIE + RS = 10 */
        return mAsI2c->writeByteData(mI2cChipAddr,DS1374_CTRL_REG,0x6d);
    }
    else
    {   /* reset value */
        return mAsI2c->writeByteData(mI2cChipAddr,DS1374_CTRL_REG,0x06);
    }
}
/*------------------------------------------------------------------------------*/
/** Acknowledge
 * @return error code
 */
int 
AsDs1374::ack(void)
{
    unsigned char rbuffer[1];

    // create an I2C write message to set register @
    mAsI2c->readByteData(mI2cChipAddr,DS1374_CNT_REG,rbuffer);
    return 0;

}

