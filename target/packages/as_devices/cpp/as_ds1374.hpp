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

#ifndef __ASDS1374_HPP__
#define __ASDS1374_HPP__

#include "as_i2c.hpp"

#define DS1374_I2C_SLAVE_ADDR 0x68
#define DS1374_CNT_REG        0x04
#define DS1374_CTRL_REG       0x07

/** AsDs1374 description
 *
 */
class AsDs1374 {
public:
    AsDs1374(int aI2cBusNum,int aI2cChipAddr);

    virtual ~AsDs1374();

    int setCounter(int aValue);
    int activate(bool state);

    int ack(void);

private:
    AsI2c * mAsI2c;
    int mI2cChipAddr;

};

#endif /* __ASDS1374_HPP__ */


