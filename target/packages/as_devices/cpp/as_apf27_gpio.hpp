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

#ifndef __ASAPF27GPIO_HPP__
#define __ASAPF27GPIO_HPP__

#include "as_apf27_gpio.h"
#define DYNAMIC_TABLE_SIZE NUMBER_OF_PORTS
#include "as_dynamic_table.hpp"

/** AsApf27Gpio description
 *
 */
class AsApf27Gpio {

public:

    static AsApf27Gpio * getInstance(char aPortLetter);
    virtual ~AsApf27Gpio();

    int setPinDirection(int aPinNum,int aDirection);
    int setPinValue(int aPinNum, int aValue);
    int getPinValue(int aPinNum);

protected:
    static AsDynamicTable * mInstances;

    AsApf27Gpio(char aPortLetter);

    int init();

    char mPortLetter; /**> Port letter in upper case */

    int mFileHandlerGpioPort;
};

#endif /* __ASAPF27GPIO_HPP__ */


