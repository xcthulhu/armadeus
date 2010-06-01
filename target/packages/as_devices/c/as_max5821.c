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

#include "as_max5821.h"
#include "as_i2c.h"


/* command bytes */
#define MAX5821_UPDATE_ALL_DAC_COMMAND       (0xE0)
#define MAX5821_EXTENDED_COMMAND_MODE        (0xF0)

#define MAX5821_POWER_CTRL_SELECTED          (0x1)
#define MAX5821_POWER_CTRL_UNSELECTED        (0x0)

#define MAX5821M_MAX_DATA_VALUE              (1023)

//#define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

/*-----------------------------------------------------------------------------*/

struct as_max5821_device *as_max5821_open(int aI2cBus, int aI2cAddress)
{
    struct as_max5821_device *dev;

    dev = (struct as_max5821_device *)malloc(sizeof(struct as_max5821_device));
    if (dev == NULL)
    {
        ERROR("can't allocate memory for device structure\n");
        return NULL;
    }

    dev->i2c_bus = as_i2c_open(aI2cBus);
    if (dev->i2c_bus < 0)
    {
        ERROR("Can't open i2c bus number %d\n",aI2cBus);
        free(dev);
        return NULL;
    }

    dev->i2c_address = aI2cAddress;

    return dev;
}

/*-----------------------------------------------------------------------------*/

int32_t as_max5821_power(struct as_max5821_device *aDev,
                         char aChannel,
                         AS_max5821_power_mode aMode)
{
    int ret;
    unsigned char buff[2];

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

    ret = as_i2c_write(aDev->i2c_bus, aDev->i2c_address, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus %d, address %d, value %x %x\n",
              aDev->i2c_bus,aDev->i2c_address,buff[0], buff[1]);
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_max5821_set_one_value(struct as_max5821_device *aDev,
                                 char aChannel,
                                 int aValue)
{
    int ret;
    unsigned char buff[2];

    switch(aChannel)
    {
        case 'a':
        case 'A': buff[0] = (aValue >> 8) & 0x0F;
                  break;
        case 'b':
        case 'B': buff[0] = 0x10 | ((aValue >> 8) & 0x0F);
                  break;
        default: ERROR("Wrong channel name %c\n", aChannel);
                 return -1;
    }

    buff[1] =(unsigned char)(aValue & 0x00FF);
    
    ret = as_i2c_write(aDev->i2c_bus, aDev->i2c_address, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus\n");
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_max5821_set_both_value(struct as_max5821_device *aDev,
                                  int aValue)
{
    int ret;
    unsigned char buff[2];

    buff[0] = 0xC0 | ((unsigned char)((aValue >> 8)) & 0x0F);
    buff[1] = (unsigned char)(aValue & 0x00FF);
    
    ret = as_i2c_write(aDev->i2c_bus, aDev->i2c_address, buff, 2);
    if (ret < 0) {
        ERROR("can't write on i2c bus\n");
        return -1;
    }

    return 0;
}

/*-----------------------------------------------------------------------------*/

int32_t as_max5821_close(struct as_max5821_device *aDev)
{
    as_i2c_close(aDev->i2c_bus);
    free(aDev);
    return 0;
}

