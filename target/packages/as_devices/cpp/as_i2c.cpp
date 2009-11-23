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
**
*/
#include "as_i2c.hpp"
#include <iostream>

AsDynamicTable * AsI2c::mInstances = new AsDynamicTable();

/** Singleton constructor
 * @param aBusNumber bus number
 */
AsI2c *
AsI2c::getInstance(int aBusNumber)
{
    AsI2c * instance;
    instance = (AsI2c *)mInstances->getInstance(aBusNumber);
    if(instance == NULL)
    {
        instance = new AsI2c(aBusNumber);
        mInstances->setInstance(instance,aBusNumber);
    }
    return instance;
}

/*------------------------------------------------------------------------------*/

AsI2c::AsI2c(int aBusNumber)
{
    int ret;

    mI2cBusNumber = aBusNumber;

    ret = as_i2c_init(aBusNumber);
    if (ret < 0)
    {
        std::cout << "AsI2C initialization error" << std::endl;
    }
}
/*------------------------------------------------------------------------------*/

AsI2c::~AsI2c()
{
    mInstances->setInstance(NULL,mI2cBusNumber);
}

/*------------------------------------------------------------------------------*/
/** read byte on i2c bus
 * @param aChipAddr chip address
 * @param aReg Register number
 * @param *aBuf char buffer
 * @return error return
 */
int 
AsI2c::readByteData(unsigned char aChipAddr, 
                       unsigned char aReg, 
                       unsigned char *aBuf )
{
    return as_read_byte_data(mI2cBusNumber, aChipAddr, aReg, aBuf);
}

/*------------------------------------------------------------------------------*/
/** write byte on i2c bus
 * @param aChipAddr chip address
 * @param aReg Register number
 * @param aBuf char value to write
 * @return error return
 */
int 
AsI2c::writeByteData(unsigned char aChipAddr,
                        unsigned char aReg, 
                        unsigned char aValue)
{
    return as_write_byte_data(mI2cBusNumber, aChipAddr, aReg, aValue);
}

/*------------------------------------------------------------------------------*/
/** write byte on i2c bus
 * @param aChipAddr chip address
 * @param aBuf char value to write
 * @return error return
 */
int 
AsI2c::writeByte(unsigned char aChipAddr,
                 unsigned char aValue)
{
    return as_write_byte(mI2cBusNumber, aChipAddr, aValue);
}

/*------------------------------------------------------------------------------*/
/** Write several byte on I2C
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
AsI2c::writeMultipleBytes(unsigned char aChipAddr,
                          unsigned char *aBuff,
                          int aSize)
{
    return as_write_multiple_bytes(mI2cBusNumber, aChipAddr, aBuff, aSize);
}

/*------------------------------------------------------------------------------*/
/** read several bytes on I2C
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
AsI2c::readMultipleBytes(unsigned char aChipAddr,
                         unsigned char *aBuff,
                         int aSize)
{
    return as_read_multiple_bytes(mI2cBusNumber, aChipAddr, aBuff, aSize);
}


