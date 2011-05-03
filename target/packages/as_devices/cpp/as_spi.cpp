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

#include "as_spi.hpp"
#include <iostream>

/** @brief Constructor: Open a SPI bus.
 *
 * @param aSpidev_name path name
 */
AsSpi::AsSpi(const unsigned char *aSpidev_name)
{
	mFd = as_spi_open(aSpidev_name);
}

AsSpi::~AsSpi()
{
	if (mFd != 0)
	{
		as_spi_close(mFd);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
	}
}

/** @brief Set bits order
 *
 * @param aLsb if 1 lsb first, else msb first
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 */
int AsSpi::setLsb(unsigned char aLsb)
{
	if (mFd != 0)
	{
		return as_spi_set_lsb(mFd, aLsb);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Get bits order
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 */
int AsSpi::getLsb() const
{
	if (mFd != 0)
	{
		return as_spi_get_lsb(mFd);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Set spi bus mode
 *
 * @param aMode	spi's bus mode
 *
 * @return mode if positive value, negative value on error
 */
int AsSpi::setMode(unsigned char aMode)
{
	if (mFd != 0)
	{
		return as_spi_set_mode(mFd, aMode);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Get spi bus mode
 *
 * @return mode if positive value, negative value on error
 */
int AsSpi::getMode() const
{
	if (mFd != 0)
	{
		return as_spi_get_mode(mFd);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Set clock bus speed
 *
 * @param aSpeed clock speed
 *
 * @return speed in Hz, negative value on error
 */
int AsSpi::setSpeed(unsigned char aSpeed)
{
	if (mFd != 0)
	{
		return as_spi_set_speed(mFd, aSpeed);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Get clock bus speed
 *
 * @return speed in Hz
 */
int AsSpi::getSpeed() const
{
	if (mFd != 0)
	{
		return as_spi_get_speed(mFd);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Get bits per word
 *
 * @return bit per word if positive value, negative value on error
 */
int AsSpi::getBitsPerWord() const
{
	if (mFd != 0)
	{
		return as_spi_get_bits_per_word(mFd);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Set bits per word
 *
 * @param aBits number of bits per word
 *
 * @return bit per word if positive value, negative value on error
 */
int AsSpi::setBitsPerWord(unsigned char aBits)
{
	if (mFd != 0)
	{
		return as_spi_set_bits_per_word(mFd, aBits);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}

/** @brief Forge arbitrary length message (31bits max) and send it
 *
 * @param aMsg right adjusted message
 * @param aLen lenght of the message
 * @param aSpeed clock speed in Hz
 *
 * @return message read
 */
unsigned long long AsSpi::msg(unsigned long long aMsg, size_t aLen, unsigned long aSpeed)
{
	if (mFd != 0)
	{
		return as_spi_msg(mFd, aMsg, aLen, aSpeed);
	}
	else
	{
		std::cerr<<"AsSpi file descriptor not initialized"<<std::endl;
		return -1;
	}
}
