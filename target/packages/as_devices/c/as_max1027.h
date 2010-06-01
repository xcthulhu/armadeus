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

#ifndef AS_MAX1027_H_
#define AS_MAX1027_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NUMBER_OF_CHANNELS (7)

typedef enum  {
    AS_MAX1027_FAST,
    AS_MAX1027_SLOW
} AS_max1027_mode;

/**
 * Store max1027 parameters
 */
struct as_max1027_device {
    AS_max1027_mode mode;
    uint8_t scan_mode;
    uint8_t fConversion;   /**< File handler for conversion register */
    uint8_t fSetup;        /**< File handler for setup register */
    uint8_t fAveraging;    /**< File handler for averaging register */
    uint8_t fLowSpeed[NUMBER_OF_CHANNELS];  /**< Files handlers for lowspeed */
    uint8_t fHighSpeed[NUMBER_OF_CHANNELS]; /**< Files handlers for high speed  */
    uint8_t fTemperature;  /**< File handler for temperature */

};

/** @brief Open max1027
 *
 * @param aSpiNum spi bus number used
 * @param aMode enum to set mode (fast or slow)
 *
 * @return as_max1027_device structure pointer, NULL if error
 */
struct as_max1027_device *as_max1027_open(int aSpiNum,
                                          AS_max1027_mode aMode);

/** @brief set averaging measurement
 *
 * @param as_max1027_device structure pointer.
 * @param aNbConv number of conversions to do for the averaging (4, 8, 16, 32). Set to 1 for disabling averaging.
 *
 * @return negative value on error and aNbConv on success
 */
int32_t as_max1027_set_averaging(struct as_max1027_device *aDev, uint8_t aNbConv);

/** @brief read temperature in milidegree ⁰C
 *
 * @param as_max1027_device structure pointer.
 * @param aTemperature pointer for temperature result.
 *
 * @return negative value on error
 *
 * @note Only slow mode can read temperature value.
 */
int32_t as_max1027_read_temperature_mC(struct as_max1027_device *aDev,
                                       int *aTemperature);

/** @brief read milivoltage value
 *
 * @param as_max1027_device structure pointer.
 * @param aChannelNum channel number to read.
 * @param *aValue pointer to returned value.
 *
 * @return negative value on error
 */
int32_t as_max1027_get_value_milliVolt(struct as_max1027_device *aDev,
                                       int aChannelNum,
                                       int *aValue);

/** @brief Close max1027
 *
 * @param aDev structure device pointer
 *
 * @return negative value on error
 */
int32_t as_max1027_close(struct as_max1027_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX1027_H_ */

