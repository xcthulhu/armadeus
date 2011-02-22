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

#include "as_adc.h"
#define NUMBER_OF_CHANNELS (7)
#define PATH_SIZE       (50)

typedef enum  {
    AS_MAX1027_FAST,
    AS_MAX1027_SLOW
} AS_max1027_mode;

struct as_max1027_device {
    AS_max1027_mode mode;
    uint8_t scan_mode;
    char path[PATH_SIZE];
};

struct as_adc_device *as_adc_open_max1027(int aDeviceNum, int aVRef);
int32_t as_adc_get_value_in_millivolts_max1027(struct as_adc_device *aDev,
                                               int aChannel);
int32_t as_adc_close_max1027(struct as_adc_device *aDev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AS_MAX1027_H_ */

