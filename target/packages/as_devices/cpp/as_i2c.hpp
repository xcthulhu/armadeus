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

#ifndef AS_I2C__HPP__
#define AS_I2C__HPP__

#include <cstring>

#include "as_i2c.h"

class AsI2c
{
public:
	AsI2c(int aBusNumber);
	virtual ~AsI2c();

	long setSlaveAddr(unsigned char aAddr);
	long getSlaveAddr() const;

	long read(unsigned char *aData, size_t aSize) const;
	long write(unsigned char *aData, size_t aSize);

	long readReg(unsigned char aReg, unsigned char *aData, size_t aSize) const;
	long writeReg(unsigned char aReg, unsigned char *aData, size_t aSize);

	long readMsg(unsigned char *aWData, unsigned char aWriteSize, unsigned char *aRData, size_t aReadSize);

	long readRegByte(unsigned char aReg) const;
	long writeRegByte(unsigned char aReg, unsigned char aVal);

protected:
	mutable struct as_i2c_device *mDev;
};

#endif // AS_I2C__HPP__
