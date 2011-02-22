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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

///XXX: find where u8 is defined under linux
typedef unsigned char u8;

#include "as_max1027.h"
#include "as_helpers.h"

/* Redundant with <linux/spi/max1027.h>, but... */
/* 00 : Scans channels 0 through N.
 * 01 : Scans channels N through the highest numbered channel.
 * 10 : Scans channel N repeatedly. The averaging register sets the number of results
 * 11 : No scan. Converts channel N once only. */
#define SCAN_MODE_00		0
#define SCAN_MODE_01		1
#define SCAN_MODE_10		2
#define SCAN_MODE_11		3
#define MAX1027_SETUP		        (0x40)
#define MAX1027_SETUP_CLKSEL(x) 	((x&0x03)<<4)
#define MAX1027_SETUP_REFSEL(x) 	((x&0x03)<<2)
#define MAX1027_SETUP_DIFFSEL(x)  	(x&0x03)
#define MAX1027_CONV                (0x80)
#define MAX1027_CONV_CHSEL(x) 		((x&0x0f)<<3)
#define MAX1027_CONV_SCAN(x)  		((x&0x03)<<1)
#define MAX1027_CONV_TEMP  			0x01
#define GET_SCAN_MODE(conv)			( (conv & 0x06) >> 1 )
#define MAX1027_AVG                 (0x20)
#define MAX1027_AVG_AVGON(x)  		((x&0x01)<<4)
#define MAX1027_AVG_NAVG(x)   		((x&0x03)<<2)
#define MAX1027_AVG_NSCAN(x)  		(x&0x03)


#define BUFFER_SIZE     (70)
#define SYS_PATH        "/sys/class/hwmon/hwmon%d/device/"
#define TEMP_NAME       "temp1_input"
#define SLOW_INPUT_NAME "in%d_input"

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

int32_t as_max1027_set_averaging(struct as_max1027_device *aDev, uint8_t aNbConv){
    uint8_t avg_register = 0;
    char buffer[BUFFER_SIZE];
    int file;
    int ret;

    switch (aNbConv)
    {
        case 1:  avg_register = MAX1027_AVG_NAVG(0) | MAX1027_AVG_AVGON(0);
                 aDev->scan_mode = SCAN_MODE_11; /* no scan */
                 break;
        case 4:  avg_register = MAX1027_AVG_NAVG(0) | MAX1027_AVG_AVGON(1);
                 aDev->scan_mode = SCAN_MODE_10; /* scan */
                 break;
        case 8:  avg_register = MAX1027_AVG_NAVG(1) | MAX1027_AVG_AVGON(1);
                 aDev->scan_mode = SCAN_MODE_10; /* scan */
                 break;
        case 16: avg_register = MAX1027_AVG_NAVG(2) | MAX1027_AVG_AVGON(1);
                 aDev->scan_mode = SCAN_MODE_10; /* scan */
                 break;
        case 32: avg_register = MAX1027_AVG_NAVG(3) | MAX1027_AVG_AVGON(1);
                 aDev->scan_mode = SCAN_MODE_10; /* scan */
                 break;
        default:
                 ERROR("%s: unsupported setting\n", __func__);
                 return -1;
    }

    /* select averaging */
    snprintf(buffer, BUFFER_SIZE, "%s%s", aDev->path, "averaging");
    file = open(buffer, O_WRONLY);
    if (file < 0) {
        ERROR("Can't open averaging file\n");
        return -1;
    }
    avg_register |= MAX1027_AVG | MAX1027_AVG_NSCAN(2);
    ret = as_write_buffer(file, avg_register);
    if (ret < 0)
    {
        ERROR("Error, setting averaging\n");
        return -1;
    }
    close(file);

    return aNbConv;
}


struct as_adc_device *as_adc_open_max1027(int aDeviceNum, int aVRef) {
    struct as_adc_device *adc_dev;
    struct as_max1027_device *dev;
    int file;
    char buffer[BUFFER_SIZE];
    int ret;

    dev = (struct as_max1027_device *)malloc(sizeof(struct as_max1027_device));

    ret = snprintf(dev->path, PATH_SIZE, SYS_PATH, aDeviceNum);
    if (ret<0)
    {
        ERROR("Error in path writing\n");
        goto err;
    }

    /* Check driver name */
    snprintf(buffer, BUFFER_SIZE, "%s%s", dev->path, "modalias");
    file = open(buffer, O_RDONLY);
    if (file < 0) {
        ERROR("Error, can't open modalias file.\n");
        goto err;
    }

    ret = read(file, buffer, BUFFER_SIZE);
    if (ret <= 0) {
        ERROR("Can't read name\n");
        goto err;
    }

    ret = strncmp(AS_MAX1027_NAME, buffer, strlen(AS_MAX1027_NAME));
    if (ret != 0) {
        ERROR("Wrong driver name %s\n", buffer);
        goto err;
    }
    close(file);

    snprintf(buffer, BUFFER_SIZE, "%s%s", dev->path, "setup");
    file = open(buffer, O_WRONLY);
    if (file < 0) {
        ERROR("Error, can't open setup file.\n");
        goto err;
    }
    ret = as_write_buffer(file, MAX1027_SETUP |
                                MAX1027_SETUP_CLKSEL(0) |
                                MAX1027_SETUP_REFSEL(0) |
                                MAX1027_SETUP_DIFFSEL(0));
    if (ret < 0) {
        ERROR("Error, can't set default setup parameters\n");
        goto err;
    }
    close(file);

    adc_dev = (struct as_adc_device *)malloc(sizeof(struct as_adc_device));

    dev->mode = AS_MAX1027_FAST ;
    dev->scan_mode = SCAN_MODE_11;

    adc_dev->chip_param = (void *)dev;
    adc_dev->device_type = "max1027";
    adc_dev->device_num = aDeviceNum;
    adc_dev->vref = 2500; /* Voltage reference is always 2.5V */

    as_max1027_set_averaging(dev, 4);
    return adc_dev;

err:
    free(dev);
    return NULL;
}

int32_t as_adc_get_value_in_millivolts_max1027(struct as_adc_device *aDev, int aChannel) {
    int32_t ret;
    int32_t value;
    int file;
    char buffer[BUFFER_SIZE];
    struct as_max1027_device *dev;

    dev = (struct as_max1027_device *)aDev->chip_param;

    if ((aChannel >= NUMBER_OF_CHANNELS) || (aChannel < 0)) {
        ERROR("Wrong num channel\n");
        return -1;
    }

    /* launch conversion */
    snprintf(buffer, BUFFER_SIZE, "%s%s", dev->path, "conversion");
    file = open(buffer, O_WRONLY);
    if (file < 0) {
        ERROR("Can't open conversion file\n");
        return -1;
    }
    ret = as_write_buffer(file, MAX1027_CONV |
                                MAX1027_CONV_CHSEL(aChannel) |
                                MAX1027_CONV_SCAN(dev->scan_mode));
    if (ret < 0) {
        ERROR("Error launching conversion\n");
        return -1;
    }
    close(file);

    /* read value */
    snprintf(buffer, BUFFER_SIZE, "%sin%d_input", dev->path, aChannel);
    file = open(buffer, O_RDONLY);
    if (file < 0) {
        ERROR("Can't open in%d_input file\n", aChannel);
        return -1;
    }
    ret = as_read_int(file, &value);
    if (ret < 0) {
        ERROR("Can't read input file\n");
        return -1;
    }
    close(file);

    return value;
}

int32_t as_adc_close_max1027(struct as_adc_device *aDev) {
    struct as_max1027_device *dev =
        (struct as_max1027_device *)aDev->chip_param;

    free(dev);
    return 0;
}


/**************************/
/* old Not used functions */
/**************************/

//int32_t as_max1027_read_temperature_mC(struct as_max1027_device *aDev,
//                                       int *aTemperature)
//{
//    int32_t ret;
//
//    /* temperature is read only on slow mode */
//    if (aDev->mode != AS_MAX1027_SLOW)
//        return -1;
//
//    /* launch conversion */
//    ret = as_write_buffer(aDev->fConversion,
//                                   MAX1027_CONV |
//                                   MAX1027_CONV_CHSEL(0) |
//                                   MAX1027_CONV_SCAN(aDev->scan_mode) |
//                                   MAX1027_CONV_TEMP);
//    if (ret < 0)
//    {
//        ERROR("Error launching conversion\n");
//        return -1;
//    }
//
//    /* read value */
//    ret =  as_read_int(aDev->fTemperature, aTemperature);
//    if (ret < 0)
//    {
//        ERROR("Error, reading fTemperature\n");
//        return -1;
//    }
//
//    return 0;
//}



