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
#include "as_apf27_gpio.hpp"

AsDynamicTable * AsApf27Gpio::mInstances = new AsDynamicTable();

/*------------------------------------------------------------------------------*/
/** Singleton constructor
 * @param aPortLetter port letter in upper case
 * @return object
 */
AsApf27Gpio *
AsApf27Gpio::getInstance(char aPortLetter)
{
    AsApf27Gpio * instance;
    
    instance = (AsApf27Gpio *)mInstances->getInstance(aPortLetter-'A');
    if(instance == NULL)
    {
        instance = new AsApf27Gpio(aPortLetter);
        mInstances->setInstance((void *)instance, aPortLetter-'A');
    }
    
    return instance;
}

/*------------------------------------------------------------------------------*/

AsApf27Gpio::AsApf27Gpio(char aPortLetter)
{
    mPortLetter = aPortLetter;
    as_apf27_gpio_init(aPortLetter);
}
/*------------------------------------------------------------------------------*/

AsApf27Gpio::~AsApf27Gpio()
{
    as_apf27_gpio_close(mPortLetter);
}

/*------------------------------------------------------------------------------*/

/** Init Gpio
 * @return AsApf27GpioError
 */
int
AsApf27Gpio::init()
{
   return as_apf27_gpio_init(mPortLetter);
}

/*------------------------------------------------------------------------------*/

/** Set pin direction
 * @param aPinNum : pin number
 * @param aDirection : direction 0:input 1:output
 * @return AsApf27GpioError
 */
int
AsApf27Gpio::setPinDirection(int aPinNum, int aDirection)
{
    return as_apf27_gpio_set_pin_value(mPortLetter, aPinNum, aDirection);
}

/*------------------------------------------------------------------------------*/

/** Set pin value 
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 * @return AsApf27GpioError
 */
int
AsApf27Gpio::setPinValue(int aPinNum, int aValue)
{
    return as_apf27_gpio_set_pin_value(mPortLetter, aPinNum, aValue);
}

/*------------------------------------------------------------------------------*/

/** Set pin value 
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 * @return AsApf27GpioError
 */

int
AsApf27Gpio::getPinValue(int aPinNum)
{
    return as_apf27_gpio_get_pin_value(mPortLetter, aPinNum);
}
