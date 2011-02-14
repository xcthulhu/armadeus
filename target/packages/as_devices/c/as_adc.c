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

#define BUFF_SIZE (300)

#define AS_MAX1027_NAME "max1027"

#define AS_AS1531_NAME      "as1531"
#define AS_AS1531_MAX_VALUE 2500

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
/*------------------------------------------------------------------------------*/

struct as_adc_device *as_adc_open_max1027(int aDeviceNum, int aVRef) {
    ERROR("Max1027 not yet supported by as_adc class\n");
    return NULL;
}

struct as_adc_device *as_adc_open_as1531(int aDeviceNum, int aVRef) {
    struct as_adc_device *dev;
    int ret;
    int fname;
    char buf[BUFF_SIZE];

    snprintf(buf, BUFF_SIZE, "/sys/class/hwmon/hwmon%d/device/name", aDeviceNum);
    fname = open(buf, O_RDONLY);
    if (fname < 0) {
        ERROR("Can't open file %s, is as1531 modprobed ?\n", buf);
        return NULL;
    }

    ret = read(fname, buf, BUFF_SIZE);
    if (ret <= 0) {
        ERROR("Can't read name\n");
        return NULL;
    }

    ret = strncmp(AS_AS1531_NAME, buf, strlen(AS_AS1531_NAME));
    if (ret != 0) {
        ERROR("Wrong driver name %s\n",buf);
        return NULL;
    }

    close(fname);

    dev = malloc(sizeof(struct as_adc_device));
    if (dev == NULL) {
        ERROR("Can't allocate memory for as_adc structure\n");
        return NULL;
    }

    dev->device_type = AS_AS1531_TYPE;
    dev->device_num  = aDeviceNum;
    dev->vref = aVRef;
    return dev;
}

struct as_adc_device *as_adc_open(int aAdcType, int aDeviceNum, int aVRef) {

    switch(aAdcType) {
        case AS_MAX1027_TYPE: return as_adc_open_max1027(aDeviceNum, aVRef);
        case AS_AS1531_TYPE : return as_adc_open_as1531(aDeviceNum, aVRef);
        default : ERROR("Undefined ADC type %d\n",aAdcType);
                  return NULL;
    }
}

int32_t as_adc_get_value_in_millivolts_max1027(struct as_adc_device *aDev, int aChannel) {
    ERROR("max1027 not supported yet\n");
    return -1;
}

int32_t as_adc_get_value_in_millivolts_as1531(struct as_adc_device *aDev, int aChannel) {
    int ret;
    int finput;
    int value;
    char buf[BUFF_SIZE];

    snprintf(buf, BUFF_SIZE, "/sys/class/hwmon/hwmon%d/device/in%d_input", aDev->device_num, aChannel);
    finput = open(buf, O_RDONLY);
    if (finput < 0) {
        ERROR("Can't open file %s.\n", buf);
        return -1;
    }

    ret = as_read_int(finput, &value);
    if (ret < 0) {
        ERROR("Can't read value on channel %d\n", aChannel);
        return -1;
    }
    close(finput);

    if (AS_AS1531_MAX_VALUE == aDev->vref) return value;

    return (value*aDev->vref)/AS_AS1531_MAX_VALUE;
}

int32_t as_adc_get_value_in_millivolts(struct as_adc_device *aDev, int aChannel) {
    switch(aDev->device_type) {
        case AS_MAX1027_TYPE: return as_adc_get_value_in_millivolts_max1027(aDev, aChannel);
        case AS_AS1531_TYPE : return as_adc_get_value_in_millivolts_as1531(aDev,  aChannel);
        default : ERROR("Undefined ADC type %d\n", aDev->device_type);
                  return -1;
    }
}

int32_t as_adc_close(struct as_adc_device *aDev) {
    return 0;
}

