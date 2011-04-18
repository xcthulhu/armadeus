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

#ifndef AS_GPIO__HPP__
#define AS_GPIO__HPP__

#include "as_gpio.h"

class AsGpio
{
public:
	AsGpio(char aPortChar, int aPinNum);
	virtual ~AsGpio();

	long setPinDirection(int aDirection);
	long getPinDirection() const;

	long setPinValue(int aValue);
	long getPinValue() const;
	long blockingGetPinValue(int aDelay_s, int aDelay_us) const;
	
	long setIrqMode(int aMode);
	long getIrqMode() const;

	long getPinNum() const;

	long getPortLetter() const;

protected:
	mutable struct as_gpio_device *mDev;
};

#endif // AS_GPIO__HPP__
