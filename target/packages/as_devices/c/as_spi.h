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
#endif // __cplusplus

#include <inttypes.h>
#include <sys/types.h>

/** @brief Open a SPI bus.
 *
 * @return the file descriptor of the opened device, -1 on error
 */
int as_spi_open(const unsigned char *spidev_name);

/** @brief Set spi bus mode
 *
 * @param fd spidev file handler
 * @param  mode
 *
 * @return mode if positive value, negative value on error
 *
 */
int as_spi_set_mode(int fd, uint8_t mode);

/** @brief Set bits order
 *
 * @param fd spidev file handler
 * @param lsb if 1 lsb first, else msb first
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 */
int as_spi_set_lsb(int fd, uint8_t lsb);

/** @brief Set bits per word
 *
 * @param fd spidev file handler
 * @param bits number of bits per word
 *
 * @return bit per word if positive value, negative value on error
 */
int as_spi_set_bits_per_word(int fd, uint8_t bits);

/** @brief Get spi bus mode
 *
 * @param fd spidev file handler
 *
 * @return mode if positive value, negative value on error
 *
 */
int as_spi_get_mode(int fd);

/** @brief Get bits order
 *
 * @param fd spidev file handler
 *
 * @return lsb first if positive, msb first if 0, negative value on error
 */
int as_spi_get_lsb(int fd);

/** @brief Get bits per word
 *
 * @param fd spidev file handler
 *
 * @return bit per word if positive value, negative value on error
 */
int as_spi_get_bits_per_word(int fd);

/** @brief Forge arbitrary length message (32bits max)
 *
 * @param fd spidev file handler
 * @param msg right adjusted message
 * @param len lenght of the message
 * @param speed clock speed in Hz
 *
 * @return message read
 */
uint32_t as_spi_forge_msg(int fd, 
                          uint32_t msg, 
                          size_t len,
                          uint32_t speed);

/** @brief Close a SPI bus.
 *
 */
void as_spi_close(int fd);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // AS_SPI_H_

