/*
**    THE ARMadeus Systems
** 
**    Copyright (C) 2009  The armadeus systems team 
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

#ifndef __ASAPF27GPIO_H__
#define __ASAPF27GPIO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define NUMBER_OF_PORTS 6

/** @brief Initialize port access
 *
 * @param aPortChar character port in UPPER case
 *
 * @return error if negative value 
 */
int32_t as_gpio_init(char aPortChar);

/** @brief  Set pin direction
 *
 * @param aPortChar port character in upper case
 * @param aPinNum pin number
 * @param aDirection direction 0:input 1:output
 *
 * @return error if negative value 
 */
int32_t as_gpio_set_pin_direction(char aPortChar,
                                    int aPinNum,
                                    int aDirection);

/** @brief Set pin value 
 *
 * @param aPortChar port character in upper case
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative 
 */
int32_t as_gpio_set_pin_value(char aPortChar,
                                int aPinNum,
                                int aValue);

/** @brief Get pin value
 *
 * @param aPortChar port character in upper case
 * @param aPinNum pin number
 *
 * @return pin value if positive or null, error if negative
 */
int32_t as_gpio_get_pin_value(char aPortChar,
                                int aPinNum);

/** @brief Close port access
 *
 * @param aPortChar port character in upper case
 *
 * @return pin value if positive or null, error if negative
 */

int32_t as_gpio_close(char aPortChar);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ASAPF27GPIO_H__
