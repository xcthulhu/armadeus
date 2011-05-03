/*
**    THE ARMadeus Systems
** 
**    Copyright (C) 2011  The armadeus systems team 
**    Jérémie Scheer <jeremie.scheeer@armadeus.com>
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

#include "as_gpio.hpp"
#include <iostream>

/** @brief Constructor: Initialize port access
 *
 * @param aPortChar character port in UPPER case
 * @param aPinNum int pin number
 */
AsGpio::AsGpio(char aPortChar, int aPinNum)
{
	mDev = as_gpio_open(aPortChar, aPinNum);
}	

AsGpio::~AsGpio()
{
	int ret;

	if (mDev != NULL)
	{
		ret = as_gpio_close(mDev);
		if (ret < 0)
		{
			std::cout<<"AsGpio destruction error"<<std::endl;
		}
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
	}
}

/** @brief  Set pin direction
 *
 * @param aDirection direction 0:input 1:output
 *
 * @return error if negative value
 */
long AsGpio::setPinDirection(int aDirection)
{
	if (mDev != NULL)
	{
		return as_gpio_set_pin_direction(mDev, aDirection);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief  Get pin direction
 *
 * @return pin direction if positive or null, error if negative
 */
long AsGpio::getPinDirection() const
{
	if (mDev != NULL)
	{
		return as_gpio_get_pin_direction(mDev);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Set pin value
 *
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative
 */
long AsGpio::setPinValue(int aValue)
{
	if (mDev != NULL)
	{
		return as_gpio_set_pin_value(mDev, aValue);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get pin value
 *
 * @return pin value if positive or null, error if negative
 */
long AsGpio::getPinValue() const
{
	if (mDev != NULL)
	{
		return as_gpio_get_pin_value(mDev);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get pin value, blocking until interrupt occur
 *
 * @param aDelay_s waiting delay in seconds
 * @param aDelay_us waiting delay in useconds (plus delay in seconds)
 *
 * @return pin value if positive or null, read error if -1, timeout if -10
 */
long AsGpio::blockingGetPinValue(int aDelay_s, int aDelay_us) const
{
	if (mDev != NULL)
	{
		return as_gpio_blocking_get_pin_value(mDev, aDelay_s, aDelay_us);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}
	
/** @brief Set pin irq mode
 *
 * @param aMode irq mode
 *
 * @return error if negative
 */
long AsGpio::setIrqMode(int aMode)
{
	if (mDev != NULL)
	{
		return as_gpio_set_irq_mode(mDev, aMode);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get pin irq mode value
 *
 * @return pin mode value if positive or null, error if negative
 */
long AsGpio::getIrqMode() const
{
	if (mDev != NULL)
	{
		return as_gpio_get_irq_mode(mDev);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get pin number value
 *
 * @return pin number, error if negative
 */
long AsGpio::getPinNum() const
{
	if (mDev != NULL)
	{
		return as_gpio_get_pin_num(mDev);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get port letter
 *
 * @return port letter, error if negative
 */
long AsGpio::getPortLetter() const
{
	if (mDev != NULL)
	{
		return as_gpio_get_port_letter(mDev);
	}
	else
	{
		std::cerr<<"AsGpio device structure not allocated"<<std::endl;
		return -1;
	}
}
