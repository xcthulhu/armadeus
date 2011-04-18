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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */
#include <sys/select.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/ppdev.h>

#include "as_helpers.h"
#include "as_adc.h"

#include "as_max1027.h"
#include "as_as1531.h"

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

struct as_adc_device *as_adc_open(const char *aAdcType, int aDeviceNum, int aVRef) {
    if (strncmp(AS_MAX1027_NAME, aAdcType, strlen(AS_MAX1027_NAME)) == 0)
        return as_adc_open_max1027(aDeviceNum, aVRef);
    if (strncmp(AS_AS1531_NAME, aAdcType, strlen(AS_AS1531_NAME)) == 0)
        return as_adc_open_as1531(aDeviceNum, aVRef);

    ERROR("Undefined ADC type '%s'\n",aAdcType);
    return NULL;
}

int32_t as_adc_get_value_in_millivolts(struct as_adc_device *aDev, int aChannel) {
    if (strncmp(AS_MAX1027_NAME, aDev->device_type, strlen(AS_MAX1027_NAME)) == 0)
        return as_adc_get_value_in_millivolts_max1027(aDev, aChannel);

    if (strncmp(AS_AS1531_NAME, aDev->device_type, strlen(AS_AS1531_NAME)) == 0)
        return as_adc_get_value_in_millivolts_as1531(aDev, aChannel);

    ERROR("Undefined ADC type '%s'\n", aDev->device_type);
    return -1;
}

int32_t as_adc_close(struct as_adc_device *aDev) {
    int ret;
    if (strncmp(AS_MAX1027_NAME, aDev->device_type, strlen(AS_MAX1027_NAME)) == 0) {
        ret = as_adc_close_max1027(aDev);
        free(aDev);
        return ret;
    }
    if (strncmp(AS_AS1531_NAME, aDev->device_type, strlen(AS_AS1531_NAME)) == 0) {
        ret = as_adc_close_as1531(aDev);
        free(aDev);
        return ret;
    }

    ERROR("Undefined ADC type '%s'\n", aDev->device_type);
    return -1;
}

