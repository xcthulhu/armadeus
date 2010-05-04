/*
**    The ARMadeus Project
**
**    Copyright (C) 2009-2010  The armadeus systems team
**    Fabien Marteau <fabien.marteau@armadeus.com>
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
*/

#ifndef __ASGPIO_H__
#define __ASGPIO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//TODO: manage irq

/**
 * Store gpio parameters
 */
struct as_gpio_device {
    unsigned char port_letter;
    int fdev;
};

/** @brief Initialize port access
 *
 * @param aPortChar character port in UPPER case
 *
 * @return error if negative value
 */
struct as_gpio_device *as_gpio_open(char aPortChar);

/** @brief  Set pin direction
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 * @param aDirection direction 0:input 1:output
 *
 * @return error if negative value
 */
int32_t as_gpio_set_pin_direction(struct as_gpio_device *aDev,
                                  int aPinNum,
                                  int aDirection);

/** @brief Set pin value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative
 */
int32_t as_gpio_set_pin_value(struct as_gpio_device *aDev,
                              int aPinNum,
                              int aValue);

/** @brief Get pin value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 *
 * @return pin value if positive or null, error if negative
 */
int32_t as_gpio_get_pin_value(struct as_gpio_device *aDev,
                              int aPinNum);

/** @brief Get pin pull-up value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 *
 * @return pin value if positive or null, error if negative
 */
int32_t as_gpio_get_pullup_value(struct as_gpio_device *aDev,
                              int aPinNum);

/** @brief Set pin pull-up value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative
 */
int32_t as_gpio_set_pullup_value(struct as_gpio_device *aDev,
                              int aPinNum,
                              int aValue);

/** @brief Close port access
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin value if positive or null, error if negative
 */

int32_t as_gpio_close(struct as_gpio_device *aDev);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ASGPIO_H__
