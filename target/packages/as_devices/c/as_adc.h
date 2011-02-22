/*
**    The ARMadeus Project
**
**    Copyright (C) 2011  The armadeus systems team
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

#ifndef __ASADC_H__
#define __ASADC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AS_MAX1027_NAME     "max1027"
#define AS_AS1531_NAME      "as1531"

/**
 * Store adc parameters
 */
struct as_adc_device {
    const char *device_type; /* Name of device type as1531 or max1027 */
    int device_num;
    int vref;         /* reference tension in millivolts */
    void *chip_param; /* chip specific parameters */
};

/** @brief Initialize port access
 *
 * @param aDeviceNum device number
 * @param aAdcType ADC type 0:max1027 1:as1531
 * @param aVRef reference tension in millivolts
 *
 * @return error if negative value
 */
struct as_adc_device *as_adc_open(const char *aAdcType, int aDeviceNum, int aVRef);

/** @brief get value in millivolts
 *
 * @param aDev as_gpio_device pointer structure
 *
 * @return error if negative millivolts
 */
int32_t as_adc_get_value_in_millivolts(struct as_adc_device *aDev, int aChannel);

/** @brief Close device
 *
 * @param Adev as_adc_device pointer structure
 *
 * @return error if negative
 */
int32_t as_adc_close(struct as_adc_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ASADC_H__ */
