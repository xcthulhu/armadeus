/*
**    The ARMadeus Project
** 
**    Copyright (C) 2009  The armadeus systems team 
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

#include "as_gpio.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */

#include <sys/ioctl.h>
#include <linux/ppdev.h> 

#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int) 
#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int) 

#define GPIO_BASE_PORT ("/dev/gpio/port")

#ifdef APF9328
#   define NUMBER_OF_PORTS 4
#elif defined(APF27)
#   define NUMBER_OF_PORTS 6
#else
#error Error no platform defined
#endif

/*------------------------------------------------------------------------------*/

struct as_gpio_device *
as_gpio_open(char aPortChar)
{
    struct as_gpio_device *dev;
    char gpio_file_path[50];
    int ret=0;

    if (((aPortChar-'A') > (NUMBER_OF_PORTS-1)) || ((aPortChar-'A') < 0))
    {
        return NULL;
    }

    /* make gpio port string path */
    ret = snprintf(gpio_file_path,50, "%s%c",
                            GPIO_BASE_PORT, aPortChar);
    if (ret < 0) {
        return NULL;
    }

    /* opening gpio port */
    ret = open(gpio_file_path, O_RDWR);
    if (ret < 0) {
        return NULL;
    }

    dev = (struct as_gpio_device *)malloc(sizeof(struct as_gpio_device)); 
    if (dev == NULL)
        return NULL;
    
    dev->port_letter = aPortChar;
    dev->fdev = ret;

    return dev;
}

/*------------------------------------------------------------------------------*/

int32_t
as_gpio_set_pin_direction(struct as_gpio_device *aDev,
                          int aPinNum,
                          int aDirection)
{
    int ret=0;
    int portval;

    /* Set LED PIN as GPIO; read/modify/write */
    ret = ioctl(aDev->fdev, GPIORDMODE, &portval);
    if (ret < 0) {
        return ret;
    }

    portval |= (1 << aPinNum);

    ret = ioctl(aDev->fdev, GPIOWRMODE, &portval);
    if (ret < 0) {
        return ret;
    }

    /* set direction */
    ret = ioctl(aDev->fdev, GPIORDDIRECTION, &portval);
    if (ret < 0) {
        return ret;
    }

    if (aDirection == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }

    ret = ioctl(aDev->fdev, GPIOWRDIRECTION, &portval);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t 
as_gpio_set_pin_value( struct as_gpio_device *aDev,
                       int aPinNum,
                       int aValue)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDDATA, &portval);
    if (ret < 0) {
        return ret;
    }

    if (aValue == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }
    ret = ioctl(aDev->fdev, GPIOWRDATA, &portval);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_value(  struct as_gpio_device *aDev,
                                int aPinNum)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDDATA, &portval);
    if (ret < 0) {
        return ret;
    }

    if ((portval & (1<<aPinNum)) != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*------------------------------------------------------------------------------*/

int32_t 
as_gpio_close(struct as_gpio_device *aDev)
{
    int ret;
    ret = close(aDev->fdev);
    free(aDev);
    return 0;
}
