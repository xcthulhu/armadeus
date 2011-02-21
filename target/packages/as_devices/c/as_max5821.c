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

#include <stdio.h>
#include <stdlib.h>

#include "as_dac.h"
#include "as_max5821.h"
#include "as_i2c.h"


/* command bytes */
#define MAX5821_UPDATE_ALL_DAC_COMMAND       (0xE0)
#define MAX5821_EXTENDED_COMMAND_MODE        (0xF0)

#define MAX5821_POWER_CTRL_SELECTED          (0x1)
#define MAX5821_POWER_CTRL_UNSELECTED        (0x0)

#define MAX5821M_MAX_DATA_VALUE              (1023)

//#define DEBUG

#ifdef DEBUG
#   define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#   define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#endif

/*-----------------------------------------------------------------------------*/

struct as_dac_device *as_dac_open_max5821(int aI2cBus, int aI2cAddress, int aVRef)
{
    struct as_max5821_device *max5821_dev;
    struct as_dac_device *dev;
    struct as_i2c_device *i2c_dev;
    int ret;

    i2c_dev = as_i2c_open(aI2cBus);
    if (i2c_dev == NULL) {
        ERROR("Can't open i2c bus number %d\n", aI2cBus);
        return NULL;
    }

    ret = as_i2c_set_slave_addr(i2c_dev, aI2cAddress);
    if (ret < 0) {
        ERROR("Can't set i2c address 0x%02x\n",aI2cAddress);
        goto close_i2c_err;
    }

    dev = (struct as_dac_device *)malloc(sizeof(struct as_dac_device));
    if (dev == NULL) {
        ERROR("can't allocate memory for device structure\n");
        goto close_i2c_err;
    }
    max5821_dev = (struct as_max5821_device *)malloc(sizeof(struct as_max5821_device));
    if (max5821_dev == NULL) {
        ERROR("can't allocate memory for device structure\n");
        goto free_dev_err;
    }

    /* TODO: power channels */

    max5821_dev->i2c_dev = i2c_dev;

    dev->bus_number = aI2cBus;
    dev->chip_address = aI2cAddress;
    dev->device_type = AS_MAX5821_TYPE;
    dev->vref = aVRef;
    dev->chip_param = (void *)max5821_dev;

    return dev;

/*free_max5821_dev_err: */
    free(max5821_dev);
free_dev_err:
    free(dev);
close_i2c_err:
    as_i2c_close(i2c_dev);
    return NULL;
}

/*-----------------------------------------------------------------------------*/

int32_t as_dac_max5821_power(struct as_dac_device *aDev,
                         char aChannel,
                         AS_max5821_power_mode aMode)
{
    struct as_max5821_device *max5821_dev;
    int ret;
    unsigned char buff[2];

    max5821_dev = aDev->chip_param;

    switch(aChannel)
    {
        case 'a':
        case 'A': buff[1] = (1<<2) | aMode;
                  break;
        case 'b':
        case 'B': buff[1] = (1<<3) | aMode;
                  break;
        default: ERROR("Wrong channel name %c\n", aChannel);
                 return -1;
    }

    buff[0] = MAX5821_EXTENDED_COMMAND_MODE;

    ret = as_i2c_write(max5821_dev->i2c_dev, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus %d, address %d, value %x %x\n",
              aDev->bus_number, aDev->chip_address,buff[0], buff[1]);
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_dac_set_value_max5821(struct as_dac_device *aDev,
                                 char aChannel,
                                 int aValue)
{
    struct as_max5821_device *max5821_dev;
    int ret;
    unsigned char buff[2];

    max5821_dev = aDev->chip_param;

    switch(aChannel)
    {
        case 'a':
        case 'A': buff[0] = (aValue >> 6) & 0x0F;
                  break;
        case 'b':
        case 'B': buff[0] = 0x10 | ((aValue >> 6) & 0x0F);
                  break;
        default: ERROR("Wrong channel name %c\n", aChannel);
                 return -1;
    }

    buff[1] =(unsigned char)((aValue << 2) & 0x00FC);

    ret = as_i2c_write(max5821_dev->i2c_dev, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus\n");
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_max5821_set_both_value(struct as_dac_device *aDev,
                                  int aValue)
{
    struct as_max5821_device *max5821_dev;
    int ret;
    unsigned char buff[2];

    max5821_dev = aDev->chip_param;

    buff[0] = 0xC0 | ((unsigned char)((aValue >> 6)) & 0x0F);
    buff[1] = (unsigned char)((aValue << 2) & 0x00FC);

    ret = as_i2c_write(max5821_dev->i2c_dev, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus\n");
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_dac_close_max5821(struct as_dac_device *aDev)
{
    struct as_max5821_device *max5821_dev;

    max5821_dev = (struct as_max5821_device *)(aDev->chip_param);
    as_i2c_close(max5821_dev->i2c_dev);
    free(max5821_dev);
    aDev->chip_param = NULL;
    return 0;
}

