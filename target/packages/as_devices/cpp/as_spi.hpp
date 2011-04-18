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

#ifndef AS_SPI__HPP__
#define AS_SPI__HPP__

#include <cstring>

#include "as_spi.h"

class AsSpi
{
	AsSpi(const unsigned char *aSpidev_name);
	virtual ~AsSpi();

	int setLsb(unsigned char aLsb);
	int getLsb() const;

	int setMode(unsigned char aMode);
	int getMode() const;

	int setSpeed(unsigned char aSpeed);
	int getSpeed() const;

	int getBitsPerWord() const;
	int setBitsPerWord(unsigned char aBits);

	unsigned long long msg(unsigned long long aMsg, size_t aLen, unsigned long aSpeed);

protected:
	int mFd;
};

#endif // AS_SPI__HPP__
