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

/** @file
 * @brief Read/Write analog value with max1027 chip 
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  {
    AS_MAX1027_FAST,
    AS_MAX1027_SLOW
} AS_max1027_mode;

/**
 * Store max1027 parameters
 */
struct as_max1027_device {
    AS_max1027_mode mode;
    int fConversion;   /**< File handler for conversion register */
    int fSetup;        /**< File handler for setup register */
    int fAveraging;    /**< File handler for averaging register */
    int fLowSpeed[8];  /**< Files handlers for lowspeed input interface */
    int fHighSpeed[8]; /**< Files handlers for high speed input interface */
    int fTemperature;  /**< File handler for temperature */
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

/** @brief Close max1027
 *
 * @param aDev structure device pointer
 *
 * @return negative value on error
 */
int32_t as_max1027_close(struct as_max1027_device *aDev);


/** @brief set averaging measurement
 *
 * @param 
 *
 * @return negative value on error
 */
int32_t as_max1027_set_averaging(struct as_max1027_device *aDev);

/** @brief read temperature in milidegree ⁰C
 *
 * @param
 *
 * @return negative value on error
 *
 * @note Only slow mode can read temperature value.
 */
int32_t as_max1027_read_temperature_mC(struct as_max1027_device *aDev);

/** @brief read milivoltage value
 *
 * @param
 *
 * @return negative value on error
 */
int32_t as_max1027_get_value_milliVolt(struct as_max1027_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX1027_H_ */
