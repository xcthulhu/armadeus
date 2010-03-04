/*
**    THE ARMadeus Systems
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


static int mFileHandlerGpioPort[NUMBER_OF_PORTS];

/*------------------------------------------------------------------------------*/

int32_t 
as_gpio_init(char aPortChar)
{
    char gpio_file_path[50];
    int ret=0;

    /* make gpio port string path */
    ret = snprintf(gpio_file_path,50, "%s%c",
                            GPIO_BASE_PORT, aPortChar);
    if (ret < 0) {
        return ret;
    }

    /* opening gpio port */
    mFileHandlerGpioPort[aPortChar-'A'] = open(gpio_file_path, O_RDWR);
    if (mFileHandlerGpioPort[aPortChar-'A'] < 0) {
        return mFileHandlerGpioPort[aPortChar-'A'];
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t
as_gpio_set_pin_direction(char aPortChar,
                                int aPinNum,
                                int aDirection)
{
    int ret=0;
    int portval;

    /* Set LED PIN as GPIO; read/modify/write */
    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIORDMODE, &portval);
    if (ret < 0) {
        return ret;
    }

    portval |= (1 << aPinNum);

    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIOWRMODE, &portval);
    if (ret < 0) {
        return ret;
    }

    /* set direction */
    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIORDDIRECTION, &portval);
    if (ret < 0) {
        return ret;
    }

    if (aDirection == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }

    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIOWRDIRECTION, &portval);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t 
as_gpio_set_pin_value(char aPortChar,
                            int aPinNum,
                            int aValue)
{
    int ret=0;
    int portval;

    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIORDDATA, &portval);
    if (ret < 0) {
        return ret;
    }

    if (aValue == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }
    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIOWRDATA, &portval);
    if (ret < 0) {
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_value(char aPortChar,
                                int aPinNum)
{
    int ret=0;
    int portval;

    ret = ioctl(mFileHandlerGpioPort[aPortChar-'A'], GPIORDDATA, &portval);
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
as_gpio_close(char aPortChar)
{
    return close(mFileHandlerGpioPort[aPortChar-'A']);
}

