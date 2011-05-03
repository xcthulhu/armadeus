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

#include "as_i2c.hpp"
#include <iostream>

/** @brief Constructor: Open an I2C bus.
 *
 * @param aBusNumber	bus to open
 */
AsI2c::AsI2c(int aBusNumber)
{
	mDev = as_i2c_open(aBusNumber);
}

AsI2c::~AsI2c()
{
	int ret;

	if (mDev != NULL)
	{
		ret = as_i2c_close(mDev);
		if (ret < 0)
		{
			std::cout<<"AsI2c destruction error"<<std::endl;
		}
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
	}
}

/** @brief Set chip's I2C slave address 
 *
 * @param aAddr		slave's address
 *
 * @return error if negative
 */
long AsI2c::setSlaveAddr(unsigned char aAddr)
{
	if (mDev != NULL)
	{
		return as_i2c_set_slave_addr(mDev, aAddr);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Get chip's I2C slave address 
 *
 * @return slave's address
 */
long AsI2c::getSlaveAddr() const
{
	if (mDev != NULL)
	{
		return as_i2c_get_slave_addr(mDev);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Read several bytes (ioctl() method) from given chip.
 *
 * @param aData		read data
 * @param aSize		data size
 *
 * @return error if negative
 */
long AsI2c::read(unsigned char *aData, size_t aSize) const
{
	if (mDev != NULL)
	{
		return as_i2c_read(mDev, aData, aSize);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Write several bytes (ioctl() method) to given chip.
 *
 * @param aData		data to write
 * @param aSize		data size
 *
 * @return error if negative
 */
long AsI2c::write(unsigned char *aData, size_t aSize)
{
	if (mDev != NULL)
	{
		return as_i2c_write(mDev, aData, aSize);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Read from given chip at a given register address (ioctl() method).
 *
 * @param aReg		register address
 * @param aData		read data
 * @param aSize		data size
 *
 * @return error if negative
 */
long AsI2c::readReg(unsigned char aReg, unsigned char *aData, size_t aSize) const
{
	if (mDev != NULL)
	{
		return as_i2c_read_reg(mDev, aReg, aData, aSize);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Write to given chip at a given register address (ioctl() method).
 *
 * @param aReg		register address
 * @param aData		data to write
 * @param aSize		data size
 *
 * @return error if negative
 */
long AsI2c::writeReg(unsigned char aReg, unsigned char *aData, size_t aSize)
{
	if (mDev != NULL)
	{
		return as_i2c_write_reg(mDev, aReg, aData, aSize);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief forge a read message like this:
 * S Addr[W] wdata0 [A] wdata1 [A] ... RS Addr R [rdata0] A [rdata1] A ... P
 *
 * @param aWData	data to write
 * @param aSize		written data size
 * @param aRData	read data
 * @param aReadSize	read data size
 *
 * @return error if negative
 */
long AsI2c::readMsg(unsigned char *aWData, unsigned char aWriteSize, unsigned char *aRData, size_t aReadSize)
{
	if (mDev != NULL)
	{
		return as_i2c_read_msg(mDev, aWData, aWriteSize, aRData, aReadSize);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Read a byte from the given register.
 *
 * @param aReg		register address
 *
 * @return read byte
 */
long AsI2c::readRegByte(unsigned char aReg) const
{
	if (mDev != NULL)
	{
		return as_i2c_read_reg_byte(mDev, aReg);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}

/** @brief Write a byte to the given register.
 *
 * @param aReg		register address
 * @param aVal		byte to write
 *
 * @return error if negative
 */
long AsI2c::writeRegByte(unsigned char aReg, unsigned char aVal)
{
	if (mDev != NULL)
	{
		return as_i2c_write_reg_byte(mDev, aReg, aVal);
	}
	else
	{
		std::cerr<<"AsI2c device structure not allocated"<<std::endl;
		return -1;
	}
}	
