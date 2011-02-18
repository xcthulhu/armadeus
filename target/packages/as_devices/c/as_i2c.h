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
 * Copyright (C) 2010, 2011  Fabien Marteau <fabien.marteau@armadeus.com> 
 *
 */

#ifndef AS_I2C_H_
#define AS_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <inttypes.h>
#include <sys/types.h>

/**
 * Store i2c parameters
 */
struct as_i2c_device {
    int i2c_id;
    int fi2c;
    uint8_t slave_addr;
};

/** @brief Open an I2C bus.
 */
struct as_i2c_device *as_i2c_open(int aBusNumber);

/** @brief Close an I2C device.
 */
int32_t as_i2c_close(struct as_i2c_device *aDev);

/** @brief Set chip's I2C slave address 
 */
int32_t as_i2c_set_slave_addr(struct as_i2c_device *aDev, uint8_t aAddr);

/** @brief Set chip's I2C slave address 
 */
int32_t as_i2c_get_slave_addr(struct as_i2c_device *aDev);

/** @brief Read several bytes (ioctl() method) from given chip.
 */
int32_t as_i2c_read(struct as_i2c_device *aDev,
                    uint8_t *aData, size_t n);

/** @brief Write several bytes (ioctl() method) to given chip.
 */
int32_t as_i2c_write(struct as_i2c_device *aDev,
                     uint8_t *aData, size_t n);

/** @brief Read from given chip at a given register address (ioctl() method).
 */
int32_t as_i2c_read_reg(struct as_i2c_device *aDev,
                        uint8_t aReg, uint8_t *aData, size_t n);

/** @brief Write to given chip at a given register address (ioctl() method).
 */
int32_t as_i2c_write_reg(struct as_i2c_device *aDev,
                         uint8_t aReg, const uint8_t *aData, size_t n);

/** @brief Read a byte from the given register.
 */
int32_t as_i2c_read_reg_byte(struct as_i2c_device *aDev, uint8_t aReg);

/** @brief Write a byte to the given register.
 */
int32_t as_i2c_write_reg_byte(struct as_i2c_device *aDev,
                              uint8_t aReg, uint8_t aVal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_I2C_H_ */

