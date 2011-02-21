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

struct as_max5821_device {
    struct as_i2c_device *i2c_dev;
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
 * @param aVRef reference voltage
 * @param aVDefault voltage required when device opened
 *
 * @return as_max5821_device structure on success, NULL on error
 */
struct as_dac_device *as_dac_open_max5821(int aI2cBus,
                                          int aI2cAddress,
                                          int aVRef);
/** @brief Power a channel
 *
 * @param aDev pointer to device structure
 * @param aChannel caracter to channel
 * @param aPowerCmd power command
 *
 * @return negative value on header.
 */
int32_t as_dac_max5821_power(struct as_dac_device *aDev,
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
int32_t as_dac_set_value_max5821(struct as_dac_device *aDev,
                                 char aChannel,
                                 int aValue);

/** @brief set both output value
 *
 * @param aDev pointer to device structure
 * @param aValue value between 0 and 1023
 *
 * @return negative value on error
 */
int32_t as_max5821_set_both_value(struct as_dac_device *aDev,
                                  int aValue);

/** @brief Close max5821
 *
 * @param aDev structure device pointer
 *
 * @return negative value on error
 */
int32_t as_dac_close_max5821(struct as_dac_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX5821_H_ */
