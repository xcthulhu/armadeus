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

#ifndef __ASI2C_HPP__
#define __ASI2C_HPP__

#include "as_i2c.h"
#define I2C_NUMBER NUMBER_OF_I2C
#define DYNAMIC_TABLE_SIZE I2C_NUMBER

#include "as_dynamic_table.hpp"

/** AsI2c description
 *
 */
class AsI2c {
public:

    static AsI2c * getInstance(int aBusNumber);

    int readByteData(unsigned char aChipAddr, 
                     unsigned char aReg, 
                     unsigned char *aBuf );

    int writeByteData(unsigned char aChipAddr,
                      unsigned char aReg, 
                      unsigned char aValue);

    int writeByte(unsigned char aChipAddr,
                  unsigned char aValue);

    int writeMultipleBytes(unsigned char aChipAddr,
                          unsigned char *aBuff,
                          int aSize);
    int readMultipleBytes(unsigned char aChipAddr,
                          unsigned char *aBuff,
                          int aSize);

    virtual ~AsI2c();

protected:
    AsI2c(int aBusNumber);

    /* multiton objects */
    static AsDynamicTable * mInstances;

    int mI2cBusNumber;

};

#endif /* __ASI2C_HPP__ */


