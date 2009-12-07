/*
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Copyright (C) 2009 Fabien Marteau <fabien.marteau@armadeus.com> 
 *
 */


#ifndef AS_SPI_H_
#define AS_SPI_H_

/** @file
 * @brief Provide access to SPI Bus.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <inttypes.h>
#include <sys/types.h>

/** @brief Open a SPI bus.
 *
 * @param aSpidev_name path name  
 *
 * @return the file descriptor of the opened device, -1 on error
 */
int as_spi_open(const unsigned char *aSpidev_name);

/** @brief Set spi bus mode
 *
 * @param aFd spidev file handler
 * @param aMode
 *
 * @return mode if positive value, negative value on error
 * TODO
 */
int as_spi_set_mode(int aFd, uint8_t aMode);

/** @brief Set bits order
 *
 * @param aFd spidev file handler
 * @param aLsb if 1 lsb first, else msb first
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 * TODO
 */
int as_spi_set_lsb(int aFd, uint8_t aLsb);

/** @brief Set bits per word
 *
 * @param aFd spidev file handler
 * @param aBits number of bits per word
 *
 * @return bit per word if positive value, negative value on error
 * TODO
 */
int as_spi_set_bits_per_word(int aFd, uint8_t aBits);

/** @brief Get spi bus mode
 *
 * @param aFd spidev file handler
 *
 * @return mode if positive value, negative value on error
 * TODO 
 */
int as_spi_get_mode(int aFd);

/** @brief Get bits order
 *
 * @param aFd spidev file handler
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 * 
 */
int as_spi_get_lsb(int aFd);

/** @brief Get clock bus speed
 *
 * @param aFd spidev file handler
 *
 * @return speed in Hz, negative value on error
 */
int as_spi_get_speed(int aFd);


/** @brief Get bits per word
 *
 * @param aFd spidev file handler
 *
 * @return bit per word if positive value, negative value on error
 */
int as_spi_get_bits_per_word(int aFd);

/** @brief Forge arbitrary length message (31bits max) and send it
 *
 * @param aFd spidev file handler
 * @param aMsg right adjusted message
 * @param aLen lenght of the message
 * @param aSpeed clock speed in Hz
 *
 * @return message read
 */
uint32_t as_spi_msg(int aFd, 
                    uint32_t aMsg, 
                    size_t aLen,
                    uint32_t aSpeed);

/** @brief Close a SPI bus.
 *
 * @param aFd file descriptor of SPI bus used
 *
 */
void as_spi_close(int aFd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_SPI_H_ */

