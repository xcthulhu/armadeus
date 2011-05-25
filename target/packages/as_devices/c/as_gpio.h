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
#endif /* __cplusplus */

#define GPIO_IRQ_MODE_NOINT    (0)
#define GPIO_IRQ_MODE_RISING   (1)
#define GPIO_IRQ_MODE_FALLING  (2)
#define GPIO_IRQ_MODE_BOTH     (3)

#define PORT_SIZE (32)

/**
 * Store gpio parameters
 */
struct as_gpio_device {
    int port_num;
    char port_letter;
    int pin_number;
    int fpin;       /* pin file for 2.6.29 interface*/
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
 * @param gpio_dev as_gpio_device pointer structure
 * @param direction "in"/"out"
 *
 * @return error if negative value
 */
int32_t as_gpio_set_pin_direction(struct as_gpio_device *gpio_dev,
                                  char *direction);

/** @brief  Get pin direction
 *
 * @param gpio_dev as_gpio_device pointer structure
 *
 * @return error if negative value
 */
const char* as_gpio_get_pin_direction(struct as_gpio_device *gpio_dev);

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

/** @brief Block until an event occur on that pin and return value
 *
 * @param aDev as_gpio_device pointer structure
 * @param aDelay_ms timeout in milliseconds
 *
 * @return pin value if positive or null, read error if -1, timeout if -10
 */
int32_t as_gpio_wait_event(struct as_gpio_device *aDev, int aDelay_ms);

/** @brief Set pin irq mode
 *
 * @param gpio_dev as_gpio_device pointer structure
 * @param mode irq mode (rising/falling/both/none)
 *
 * @return error if negative
 */
int32_t as_gpio_set_irq_mode(struct as_gpio_device *gpio_dev, char *mode);

/** @brief Get pin irq mode value
 *
 * @param gpio_dev as_gpio_device pointer structure
 *
 * @return pin triggering mode: (rising/falling/both/none)
 */
const char* as_gpio_get_irq_mode(struct as_gpio_device *gpio_dev);

/** @brief Get pin number value
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return pin number, error if negative
 */
int32_t as_gpio_get_pin_num(struct as_gpio_device *aDev);

/** @brief Get port letter
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return port letter, error if negative
 */
char as_gpio_get_port_letter(struct as_gpio_device *aDev);

/** @brief Close port access
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return error if negative
 */
int32_t as_gpio_close(struct as_gpio_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ASGPIO_H__ */
