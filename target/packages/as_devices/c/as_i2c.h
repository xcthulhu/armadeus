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
 * Copyright (C) 2009  Benoît Ryder <benoit@ryder.fr>
 *
 */


#ifndef AS_I2C_H_
#define AS_I2C_H_

/** @file
 * @brief Provide access to I2C devices.
 *
 * Once opened with as_i2c_open(), the I2C slave address to talk with should be
 * set using as_i2c_set_slave(). Then, standard \e read(), \e write() and
 * close() can be used. Otherwise, methods provided by this module can be used.
 * These methods does not use standard <em>read()/write</em> methods nor
 * combined transactions (several read/write messages in the same transaction).
 *
 * @note All methods will leave \e errno unchanged on system call errors, which
 * will happen when -1 is returned.
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <inttypes.h>
#include <sys/types.h>


/// Number of I2C busses on system
#define AS_I2C_DEV_COUNT 2


/** @brief Open an I2C bus.
 *
 * \e i2c_id is the ID of the I2C bus to open, it should strictly lower than
 * \e AS_I2C_DEV_COUNT.
 *
 * @return the file descriptor of the opened device, -1 on error.
 */
int as_i2c_open(unsigned int i2c_id);

/** @brief Close an I2C device.
 *
 * @return 0 on success, -1 on error.
 *
 * @note This is an alias of the standard \e close() system call.
 */
int as_i2c_close(int fd);

/** @brief Set I2C slave address to be used.
 *
 * @return 0 on success, -1 on error.
 */
int as_i2c_set_slave(int fd, uint8_t addr);


/** @brief Read several bytes.
 *
 * @return 0 on success, -1 on error.
 */
int as_i2c_read(int fd, uint8_t addr, uint8_t *data, size_t n);

/** @brief Write several bytes.
 *
 * @return 0 on success, -1 on error.
 */
int as_i2c_write(int fd, uint8_t addr, const uint8_t *data, size_t n);


/** @brief Read at a given register address.
 *
 * Send the \e reg byte, then read bytes.
 *
 * @return 0 on success, -1 on write error (\e reg byte), -2 on read error.
 */
int as_i2c_read_reg(int fd, uint8_t addr, uint8_t reg, uint8_t *data, size_t n);

/** @brief Write at a given register address.
 *
 * Send the \e reg byte followed by data.
 *
 * @return 0 on success, -1 on error.
 */
int as_i2c_write_reg(int fd, uint8_t addr, uint8_t reg, const uint8_t *data, size_t n);


/** @brief Read a given register byte.
 *
 * Send the \e reg byte, then read and return a single byte.
 *
 * @return the read byte, -1 on write error (\e reg byte), -2 on read error.
 *
 * @note Equivalent to \e as_i2c_read_reg() with a 1-byte buffer, but return
 * the read byte.
 */
int as_i2c_read_reg_byte(int fd, uint8_t addr, uint8_t reg);

/** @brief Write a given register byte.
 *
 * Send the \e reg byte followed by the \e val byte.
 *
 * @return 0 on success, -1 on error.
 *
 * @note Equivalent to \e as_i2c_write() with a 2-byte buffer.
 */
int as_i2c_write_reg_byte(int fd, uint8_t addr, uint8_t reg, uint8_t val);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // AS_I2C_H_

