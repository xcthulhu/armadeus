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
 * Copyright (C) 2011 Fabien Marteau <fabien.marteau@armadeus.com>
 *
 */


#ifndef AS_DAC_H_
#define AS_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "as_dac.h"

#define AS_MAX5821_TYPE "max5821"
#define AS_MCP4912

/**
 * Store dac parameters
 */
struct as_dac_device {
    int bus_number;
    int chip_address;
    int vref;
    const char *device_type;
    void *chip_param;
};

/** @brief Open dac
 *
 * @param aDacType type name of adc ("max5821" or "mcp4912")
 * @param aBusNumber bus number used
 * @param aAddress i2c address for i2c chip, and CS number for SPI chip
 * @param aVRef voltage reference in milivolt
 * @param aVDefault default voltage value at open
 *
 * @return as_dac_device structure pointer on success, NULL on error
 */
struct as_dac_device *as_dac_open(const char *aDacType, int aBusNumber,
                                  int aAddress, int aVRef);


/** @brief set a channel value.
 *
 * @param aDev structure pointer of dac device
 * @param aChannel channel number (A:0, B:1,...)
 * @param aValue channel value in milivolt
 *
 * @return negative value on error
 */
int32_t as_dac_set_value(struct as_dac_device *aDev,
                        int aChannel,
                        int aValue);

/** @brief close device
 *
 * @param aDev structure pointer of dac device
 *
 * @return negative value on error
 */
int32_t as_dac_close(struct as_dac_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_DAC_H_ */
