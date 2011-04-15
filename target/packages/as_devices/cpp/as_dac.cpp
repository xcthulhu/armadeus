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

#include "as_dac.hpp"
#include <iostream>

/** @brief Constructor: Open DAC
 *
 * @param aDacType type name of adc ("max5821" or "mcp4912")
 * @param aBusNumber bus number used
 * @param aAddress i2c address for i2c chip, and CS number for SPI chip
 * @param aVRef voltage reference in milivolt
 * @param aVDefault default voltage value at open
 */
AsDac::AsDac(const char *aDacType, int aBusNumber, int aAddress, int aVRef)
{
	mDev = as_dac_open(aDacType, aBusNumber, aAddress, aVRef);
}

AsDac::~AsDac()
{
	int ret;

	ret = as_dac_close(mDev);
	if (ret < 0)
	{
		std::cout<<"AsDac destruction error"<<std::endl;
	}
}
	
/** @brief set a channel value.
 *
 * @param aChannel channel number (A:0, B:1,...)
 * @param aValue channel value in milivolt
 *
 * @return negative value on error
 */
long AsDac::setValueInMillivolts(int aChannel, int aValue)
{
	return as_dac_set_value_in_millivolts(mDev, aChannel, aValue);
}
