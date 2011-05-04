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

#include "as_adc.hpp"
#include <iostream>

/** @brief Constructor: Initialize port access
 *
 * @param aDeviceNum device number
 * @param aAdcType ADC type 0:max1027 1:as1531
 * @param aVRef reference tension in millivolts
 */
AsAdc::AsAdc(const char *aAdcType, int aDeviceNum, int aVRef)
{
	mDev = as_adc_open(aAdcType, aDeviceNum, aVRef);
}

AsAdc::~AsAdc()
{
	int ret;

	if (mDev != NULL)
	{
		ret = as_adc_close(mDev);
		if (ret < 0)
		{
			std::cout<<"AsAdc destruction error"<<std::endl;
		}
	}
	else
	{
		std::cerr<<"AsAdc device structure not allocated"<<std::endl;
	}
}
	

/** @brief get value in millivolts
 *
 * @param aChannel	channel number
 *
 * @return value in millivolts ; error if negative millivolts
 */
long AsAdc::getValueInMillivolts(int aChannel) const
{
	if (mDev != NULL)
	{	
		return as_adc_get_value_in_millivolts(mDev, aChannel);
	}
	else
	{
		std::cerr<<"AsAdc device structure not allocated"<<std::endl;
		return -1;
	}
}
