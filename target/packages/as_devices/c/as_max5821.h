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
 * Copyright (C) 2010 Fabien Marteau <fabien.marteau@armadeus.com>
 *
 */

#ifndef AS_MAX5821_H_
#define AS_MAX5821_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "as_i2c.h"

/**
 * Store max5821 parameters
 */
struct as_max5821_device {
    int i2c_bus;
    int i2c_address;
};

typedef enum {
    MAX5821_POWER_UP         = 0, 
    MAX5821_POWER_DOWN_MODE0,
    MAX5821_POWER_DOWN_MODE1,
    MAX5821_POWER_DOWN_MODE2
} AS_max5821_power_mode;

/** @brief Open max5821
 *
 * @param aI2cBus i2c bus number
 * @param aI2cAddress i2c chip address
 *
 * @return as_max5821_device structure on success, NULL on error
 */
struct as_max5821_device *as_max5821_open(int aI2cBus, int aI2cAddress);

/** @brief Power a channel
 *
 * @param aDev pointer to device structure
 * @param aChannel caracter to channel
 * @param aPowerCmd power command
 *
 * @return negative value on header.
 */
int32_t as_max5821_power(struct as_max5821_device *aDev,
                         char aChannel,
                         AS_max5821_power_mode aMode);

/** @brief set output value
 *
 * @param aDev pointer to device structure
 * @param aChannel caracter to channel
 * @param aValue value between 0 and 1023
 *
 * @return negative value on error.
 */
int32_t as_max5821_set_one_value(struct as_max5821_device *aDev,
                                 char aChannel,
                                 int aValue);

/** @brief set both output value
 *
 * @param aDev pointer to device structure
 * @param aValue value between 0 and 1023
 *
 * @return negative value on error
 */
int32_t as_max5821_set_both_value(struct as_max5821_device *aDev,
                                  int aValue);

/** @brief Close max5821
 *
 * @param aDev structure device pointer
 *
 * @return negative value on error
 */
int32_t as_max5821_close(struct as_max5821_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX5821_H_ */
