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

#include <linux/spi/max1027.h>
#include "as_max1027.h"


#define BUFFER_SIZE     (70)
#define PATH_SIZE       (50)
#define SYS_PATH        "/sys/bus/spi/devices/spi%d.0/"
#define TEMP_NAME       "temp1_input"
#define SLOW_INPUT_NAME "in%d_input"
#define FAST_INPUT_PATH "/dev/max1027/AIN%d"

/** @brief Functiun used to write in /sys file
 *
 * @param aFile_handler /sys file handler
 * @param aValue int value to write
 *
 * @return negative value on error
 */
int32_t as_max1027_write_buffer(int aFile_handler, int aValue)
{
    int ret;
    char buffer[50];
    int buffer_len;

    ret = snprintf(buffer, 50, "%d", aValue);
    if (ret < 0)
        return -1;
    buffer_len = strlen(buffer);
    ret = write(aFile_handler, buffer, buffer_len);
    if (ret < 0)
    {
        perror("Can't write file ");
        return -1;
    }
    ret = lseek(aFile_handler,0,SEEK_SET);
    if (ret < 0)
    {
        perror("lseek error ");
        return -1;
    }

    return buffer_len;
}

/** @brief Functiun used to read int value in /sys file
 *
 * @param aFile_handler /sys file handler
 * @param aTemperature pointer to temperature read
 *
 * @return negative value on error,
 */
int32_t as_max1027_read_buffer(int aFile_handler, int *aValueRead)
{
    const int SIZEOFBUFF = 20;
    ssize_t ret;
    char valueRead[SIZEOFBUFF];
    int value;

    ret = read(aFile_handler, valueRead, SIZEOFBUFF);
    if (ret < 0)
    {
        printf("Error, reading buffer file\n");
        return -1;
    }
    valueRead[ret-1] = '\0';

    ret = lseek(aFile_handler, 0, SEEK_SET);
    if (ret < 0)
        return -1;
    value = strtol(valueRead, NULL, 10);
    *aValueRead = value;
    return 0;
}

struct as_max1027_device *as_max1027_open(int aSpiNum,
                                          AS_max1027_mode aMode)
{
    struct as_max1027_device *dev;
    int i;
    int fConversion, fSetup, fAveraging;
    int fTemperature;
    int fLowSpeed[NUMBER_OF_CHANNELS];
    int fHighSpeed[NUMBER_OF_CHANNELS];
    char path[PATH_SIZE];
    char buffer[BUFFER_SIZE];
    char slow_input_name[20];
    int ret;

    /** XXX */
    if (aMode == AS_MAX1027_FAST)
    {
        printf("Error, fast mode not supported yet\n");
        return NULL;
    }

    ret = snprintf(path, PATH_SIZE, SYS_PATH, aSpiNum);
    if (ret<0) {
        printf("Error in path writing\n");
        return NULL;
    }

    /* Open file handler register */
    /* conversion */
    ret = snprintf(buffer, BUFFER_SIZE, "%s%s", path, "conversion");
    if (ret<0) {
        printf("Error in path writing\n");
        return NULL;
    }
    fConversion = open(buffer, O_RDWR);
    if (fConversion < 0){
        printf("Error, can't open conversion file. Is max1027 modprobed ?\n");
        return NULL;
    }
    /* setup */
    ret = snprintf(buffer, BUFFER_SIZE, "%s%s", path, "setup");
    if (ret<0) {
        printf("Error in path writing\n");
        return NULL;
    }
    fSetup = open(buffer, O_RDWR);
    if (fSetup < 0){
        printf("Error, can't open setup file.\n");
        return NULL;
    }
    ret = as_max1027_write_buffer( fSetup,
                                   MAX1027_SETUP |
                                   MAX1027_SETUP_CLKSEL(0) |
                                   MAX1027_SETUP_REFSEL(0) |
                                   MAX1027_SETUP_DIFFSEL(0));
    if (ret < 0)
    {
        printf("Error, can't set default setup parameters\n");
        return NULL;
    }
    /* averaging */
    ret = snprintf(buffer, BUFFER_SIZE, "%s%s", path, "averaging");
    if (ret<0) {
        printf("Error in path writing\n");
        return NULL;
    }

    fAveraging = open(buffer, O_RDWR);
    if (fAveraging < 0){
        printf("Error, can't open averaging file.\n");
        return NULL;
    }

    /* temperature */
    if (aMode == AS_MAX1027_SLOW){
        ret = snprintf(buffer, BUFFER_SIZE, "%s%s", path, "temp1_input");
        if (ret<0) {
            printf("Error in path writing\n");
            return NULL;
        }
        fTemperature = open(buffer, O_RDONLY);
        if (fTemperature < 0){
            printf("Error, can't open temperature file.\n");
            return NULL;
        }
    } else {
        fTemperature = -1;
    }

    /* Open each channels (0 to 6)*/
    for( i=0 ; i<NUMBER_OF_CHANNELS ; i++){
        fLowSpeed[i] = -1;
        fHighSpeed[i] = -1;
        if (aMode == AS_MAX1027_SLOW)
        {
            ret = snprintf(slow_input_name, 20, SLOW_INPUT_NAME, i);
            if (ret < 0) {
                printf("Error in path writing\n");
                return NULL;
            }

            ret = snprintf(buffer, BUFFER_SIZE, "%s%s", path, slow_input_name);
            if (ret < 0) {
                printf("Error in path writing\n");
                return NULL;
            }

            fLowSpeed[i] = open(buffer, O_RDONLY);
            if (fLowSpeed[i] < 0){
                printf("Error, can't open %s\n", buffer);
                return NULL;
            }
        } else if ((aMode == AS_MAX1027_FAST)){
            ret = snprintf(buffer, BUFFER_SIZE, FAST_INPUT_PATH, i);
            if (ret < 0) {
                printf("Error in path writing\n");
                return NULL;
            }

            fHighSpeed[i] = open(buffer, O_RDONLY);
            if (fHighSpeed[i] < 0){
                printf("Error, can't open %s\nIs loadmax.sh launched ?\n", buffer);
                return NULL;
            }

        }
    }

    dev = (struct as_max1027_device *)malloc(sizeof(struct as_max1027_device));

    dev->mode = aMode;
    dev->scan_mode = SCAN_MODE_11;
    dev->fConversion = fConversion;
    dev->fSetup = fSetup;
    dev->fAveraging = fAveraging;
    for( i=0 ; i<NUMBER_OF_CHANNELS ; i++){
        dev->fLowSpeed[i] = fLowSpeed[i];
        dev->fHighSpeed[i] = fHighSpeed[i];
    }
    dev->fTemperature = fTemperature;

    return dev;
}

int32_t as_max1027_close(struct as_max1027_device *aDev)
{
    int i;

    if (aDev->fConversion > 0) close(aDev->fConversion);
    if (aDev->fSetup > 0) close(aDev->fSetup);
    if (aDev->fAveraging > 0) close(aDev->fAveraging);

    if (aDev->fTemperature > 0) close(aDev->fTemperature);

    for (i=0 ; i<NUMBER_OF_CHANNELS; i++){
        if (aDev->fLowSpeed[i] > 0) close(aDev->fLowSpeed[i]);
        if (aDev->fHighSpeed[i] > 0) close(aDev->fHighSpeed[i]);
    }

    free(aDev);

    return 0;
}

int32_t as_max1027_read_temperature_mC(struct as_max1027_device *aDev,
                                       int *aTemperature)
{
    int32_t ret;

    /* temperature is read only on slow mode */
    if (aDev->mode != AS_MAX1027_SLOW)
        return -1;

    /* launch conversion */
    ret = as_max1027_write_buffer( aDev->fConversion,
                                   MAX1027_CONV |
                                   MAX1027_CONV_CHSEL(0) |
                                   MAX1027_CONV_SCAN(aDev->scan_mode) |
                                   MAX1027_CONV_TEMP);
    if (ret < 0){
        printf("Error launching conversion\n");
        return -1;
    }

    /* read value */
    ret =  as_max1027_read_buffer(aDev->fTemperature,aTemperature);
    if (ret < 0){
        printf("Error, reading fTemperature\n");
        return -1;
    }

    return 0;
}

int32_t as_max1027_set_averaging(struct as_max1027_device *aDev, uint8_t aNbConv){
    uint8_t avg_register = 0;
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
        default: printf("%s: unsupported setting\n", __func__);
                 return -1;
    }

    /* select averaging */
    avg_register |= MAX1027_AVG | MAX1027_AVG_NSCAN(2);
    ret = as_max1027_write_buffer( aDev->fAveraging, avg_register);
    if (ret < 0){
        printf("Error, setting averaging\n");
        return -1;
    }

    return aNbConv;
}


int32_t as_max1027_get_value_milliVolt(struct as_max1027_device *aDev,
                                       int aChannelNum, int *aValue)
{
    int32_t ret;

    if ((aChannelNum >= NUMBER_OF_CHANNELS) || (aChannelNum < 0))
    {
        printf("Wrong num channel\n");
        return -1;
    }

    /* temperature is read only on slow mode */
    if (aDev->mode == AS_MAX1027_SLOW)
    {
        /* launch conversion */
        ret = as_max1027_write_buffer( aDev->fConversion,
                                       MAX1027_CONV |
                                       MAX1027_CONV_CHSEL(aChannelNum) |
                                       MAX1027_CONV_SCAN(aDev->scan_mode));
        if (ret < 0){
            printf("Error launching conversion\n");
            return -1;
        }

        /* read value */
        ret =  as_max1027_read_buffer(aDev->fLowSpeed[aChannelNum], aValue);
        if (ret < 0){
            printf("Error, reading fLowSpeed[%d]\n",aChannelNum);
            return -1;
        }
    } else if (aDev->mode == AS_MAX1027_FAST) {
        printf("TODO: fast mode channel reading\n");
        return -1;
    } else {
        printf("Uncknow mode\n");
        return -1;
    }

    return 0;
}

