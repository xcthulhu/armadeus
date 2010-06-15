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

#define GPIO_IRQ_MODE_NOINT    (0)
#define GPIO_IRQ_MODE_RISING   (1)
#define GPIO_IRQ_MODE_FALLING  (2)
#define GPIO_IRQ_MODE_BOTH     (3)

#define PORT_SIZE (32)

/**
 * Store gpio parameters
 */
struct as_gpio_device {
    unsigned char port_letter;
    int pin_num;
    int fpin;       /* pin file */
    int irq_mode;
};

/** @brief Initialize port access
 *
 * @param aPortChar character port in UPPER case
 * @param aPinNum int pin number
 *
 * @return error if negative value
 */
struct as_gpio_device *as_gpio_open(char aPortChar, int aPinNum);

/** @brief  Set pin direction
 *
 * @param aDev as_gpio_device pointer structure
 * @param aDirection direction 0:input 1:output
 *
 * @return error if negative value
 */
int32_t as_gpio_set_pin_direction(struct as_gpio_device *aDev,
                                  int aDirection);

/** @brief  Get pin direction
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return error if negative value
 */
int32_t as_gpio_get_pin_direction(struct as_gpio_device *aDev);

/** @brief Set pin value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative
 */
int32_t as_gpio_set_pin_value(struct as_gpio_device *aDev,
                              int aValue);

/** @brief Get pin value
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin value if positive or null, error if negative
 */
int32_t as_gpio_get_pin_value(struct as_gpio_device *aDev);

/** @brief Get pin value, blocking until interrupt occur
 *
 * @param aDev as_gpio_device pointer structure
 * @param aDelay_s waiting delay in seconds
 * @param aDelay_us waiting delay in useconds (plus delay in seconds)
 *
 * @return pin value if positive or null, read error if -1, timeout if -10
 */
int32_t as_gpio_blocking_get_pin_value(struct as_gpio_device *aDev,
                                       int aDelay_s,
                                       int aDelay_us);

/** @brief Get pin pull-up value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aPinNum pin number
 *
 * @return pin pull up value if positive or null, error if negative
 */
int32_t as_gpio_get_pullup_value(struct as_gpio_device *aDev);

/** @brief Set pin pull-up value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aValue value of pin (1 or 0)
 *
 * @return error if negative
 */
int32_t as_gpio_set_pullup_value(struct as_gpio_device *aDev,
                                 int aValue);

/** @brief Set pin irq mode
 *
 * @param aDev as_gpio_device pointer structure
 * @param aMode irq mode
 *
 * @return error if negative
 */
int32_t as_gpio_set_irq_mode(struct as_gpio_device *aDev,
                             int aMode);

/** @brief Get pin irq mode value
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin mode value if positive or null, error if negative
 */
int32_t as_gpio_get_irq_mode(struct as_gpio_device *aDev);

/** @brief Get pin number value
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin mode value if positive or null, error if negative
 */
int32_t as_gpio_get_pin_num(struct as_gpio_device *aDev);

/** @brief Get port letter
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin mode value if positive or null, error if negative
 */
int32_t as_gpio_get_port_letter(struct as_gpio_device *aDev);

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
