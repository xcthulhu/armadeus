/*
 **    The ARMadeus Project
 **
 **    Copyright (C) 2009-2010  The armadeus systems team
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

#include <sys/ioctl.h>
#include <linux/ppdev.h>

#include "as_gpio.h"

/* IOCTL */
/* direction: 1 output, 0 input */
#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)

#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int)

/* mode: 1 GPIO, 0 device function */
#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int)

#define GPIORDPULLUP	_IOR(PP_IOCTL, 0xF6, int)
#define GPIOWRPULLUP	_IOW(PP_IOCTL, 0xF7, int)

/* IRQ type: 0 none, 1 rising, 2 falling, 3 both */
#define GPIORDIRQMODE	_IOR(PP_IOCTL, 0xF8, int)
#define GPIOWRIRQMODE	_IOW(PP_IOCTL, 0xF9, int)


#define GPIO_BASE_PIN  ("/dev/gpio/P")

#define BUFF_SIZE (300)

//#define DEBUG
#ifdef DEBUG
#   define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#   define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#endif

/*------------------------------------------------------------------------------*/

struct as_gpio_device *as_gpio_open(char aPortChar, int aPinNum)
{
    struct as_gpio_device *dev;
    char gpio_file_path[50];
    int ret = 0;
    int file;
    int value;

    /* make gpio port string path */
    ret = snprintf(gpio_file_path,50, "%s%c%d",
                   GPIO_BASE_PIN, aPortChar, aPinNum);
    if (ret < 0) {
        ERROR("Can't forge gpio pin port path\n");
        goto path_error;
    }

    /* opening gpio port */
    file = open(gpio_file_path, O_RDWR);
    if (file < 0) {
        ERROR("Can't open file pin port\n");
        goto open_file_error;
    }

    dev = (struct as_gpio_device *)malloc(sizeof(struct as_gpio_device));
    if (dev == NULL) {
        ERROR("Can't allocate memory for gpio device structure\n");
        goto malloc_error;
    }

    dev->port_letter = aPortChar;
    dev->fpin = file;
    dev->pin_num = aPinNum;

    value = 1;
    ret = ioctl(dev->fpin, GPIOWRMODE, &value); /* set pin on gpio mode */
    if (ret < 0) {
        ERROR("can't set pin in gpio mode.\n");
        goto mode_error;
    }

    return dev;

mode_error:
    free(dev);
malloc_error:
    close(file);
open_file_error:
path_error:
    return NULL;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_direction(struct as_gpio_device *aDev, int aDirection)
{
    int ret = 0;
    int portval;

    portval = (aDirection != 0) ? 1 : 0;

    ret = ioctl(aDev->fpin, GPIOWRDIRECTION, &portval);
    if (ret < 0) {
        ERROR("Can't set gpio direction\n");
        return ret;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_direction(struct as_gpio_device *aDev)
{
    int ret = 0;
    int portval;

    ret = ioctl(aDev->fpin, GPIORDDIRECTION, &portval);
    if (ret < 0) {
        ERROR("Can't get gpio direction\n");
        return ret;
    }
    return ((portval !=0) ? 1 : 0);
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_value(struct as_gpio_device *aDev, int aValue)
{
    int ret = 0;
    int portval;

    portval = (aValue != 0) ? 1 : 0;
    ret = ioctl(aDev->fpin, GPIOWRDATA, &portval);
    if (ret < 0) {
        ERROR("Can't write gpio data\n");
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_value(struct as_gpio_device *aDev)
{
    int ret = 0;
    int portval;
    
    ret = ioctl(aDev->fpin, GPIORDDATA, &portval);
    if (ret < 0) {
        ERROR("Can't read gpio data\n");
        return ret;
    }

    if (portval != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_blocking_get_pin_value(struct as_gpio_device *aDev,
                                       int aDelay_s,
                                       int aDelay_us)
{
    int ret;
    char value;
    fd_set rfds;
    struct timeval tv;
    int retval;

    if (as_gpio_get_irq_mode(aDev) == GPIO_IRQ_MODE_NOINT) {
        ERROR("irq must be configured before\n");
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(aDev->fpin, &rfds);

    tv.tv_sec = aDelay_s;
    tv.tv_usec = aDelay_us;

    /* flush the file */
    retval = select(aDev->fpin+1, &rfds, NULL, NULL, &tv);
    if (retval > 0) {
        ret = read(aDev->fpin, &value, 1);
        if (ret < 0) {
            ERROR("Can't read pin value\n");
            return -1;
        }
        if (value != 0)
            return 1;
        else return 0;
    } else if (retval == 0) {
        return -10;
    } else {
        return -1;
    }
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pullup_value(struct as_gpio_device *aDev)
{
    int ret = 0;
    int portval;

    ret = ioctl(aDev->fpin, GPIORDPULLUP, &portval);
    if (ret < 0) {
        ERROR("can't get pullup value\n");
        return ret;
    }

    if (portval != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pullup_value(struct as_gpio_device *aDev, int aValue)
{
    int ret = 0;
    int portval;

    portval = (aValue != 0)?1:0;

    ret = ioctl(aDev->fpin, GPIOWRPULLUP, &portval);
    if (ret < 0) {
        ERROR("Can't set pullup value\n");
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_irq_mode(struct as_gpio_device *aDev)
{
    int ret = 0;
    int portval;

    ret = ioctl(aDev->fpin, GPIORDIRQMODE, &portval);
    if (ret < 0) {
        ERROR("Can't read irq mode\n");
        return ret;
    }

    return portval;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_irq_mode(struct as_gpio_device *aDev, int aMode)
{
    int ret = 0;
    int portval;
    char buffer[BUFF_SIZE];
    fd_set rfds;
    struct timeval tv;
    int retval;

    portval = aMode;

    ret = ioctl(aDev->fpin, GPIOWRIRQMODE, &portval);
    if (ret < 0) {
        ERROR("Can't write irq mode L\n");
        return ret;
    }

    FD_ZERO(&rfds);
    FD_SET(aDev->fpin, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 10;

    /* flush the file */
    retval = select(aDev->fpin, &rfds, NULL, NULL, &tv);
    if (retval > 0) {
        read(aDev->fpin, buffer, BUFF_SIZE);
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_num(struct as_gpio_device *aDev)
{
    return aDev->pin_num;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_port_letter(struct as_gpio_device *aDev)
{
    return aDev->port_letter;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_close(struct as_gpio_device *aDev)
{
    close(aDev->fpin);
    free(aDev);

    return 0;
}

