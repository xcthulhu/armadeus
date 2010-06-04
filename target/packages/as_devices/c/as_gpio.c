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

#include "as_gpio.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */
#include <sys/select.h>

#include <sys/ioctl.h>
#include <linux/ppdev.h>


#define GPIORDDIRECTION    _IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION    _IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA         _IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA         _IOW(PP_IOCTL, 0xF3, int)
#define GPIORDMODE         _IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE         _IOW(PP_IOCTL, 0xF5, int)

#define GPIORDPULLUP       _IOR(PP_IOCTL, 0xF6, int)
#define GPIOWRPULLUP       _IOW(PP_IOCTL, 0xF7, int)

#define GPIORDIRQMODE_H    _IOR(PP_IOCTL, 0xF8, int)
#define GPIORDIRQMODE_L    _IOR(PP_IOCTL, 0xF9, int)
#define GPIOWRIRQMODE_H    _IOW(PP_IOCTL, 0xFA, int)
#define GPIOWRIRQMODE_L    _IOW(PP_IOCTL, 0xFB, int)

#define GPIORDISR          _IOR(PP_IOCTL, 0xFC, int)
#define GPIOWRISR          _IOW(PP_IOCTL, 0xFD, int)

#define GPIO_BASE_PORT ("/dev/gpio/port")
#define GPIO_BASE_PIN  ("/dev/gpio/P")

#define BUFF_SIZE (300)

#ifdef APF9328
#   define NUMBER_OF_PORTS 4
#elif defined(APF27)
#   define NUMBER_OF_PORTS 6
#else
#error Error no platform defined
#endif

//#define DEBUG
#ifdef DEBUG
#   define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#   define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#endif

/*------------------------------------------------------------------------------*/

struct as_gpio_device *as_gpio_open(char aPortChar)
{
    struct as_gpio_device *dev;
    char gpio_file_path[50];
    int ret=0;
    int i;

    if (((aPortChar-'A') > (NUMBER_OF_PORTS-1)) || ((aPortChar-'A') < 0))
    {
        ERROR("No port named %c\n",aPortChar);
        return NULL;
    }

    /* make gpio port string path */
    ret = snprintf(gpio_file_path,50, "%s%c",
                   GPIO_BASE_PORT, aPortChar);
    if (ret < 0) {
        ERROR("Can't forge gpio port path\n");
        return NULL;
    }

    /* opening gpio port */
    ret = open(gpio_file_path, O_RDWR);
    if (ret < 0) {
        ERROR("Can't open file port\n");
        return NULL;
    }

    dev = (struct as_gpio_device *)malloc(sizeof(struct as_gpio_device));
    if (dev == NULL)
    {
        ERROR("Can't allocate memory for gpio device structure\n");
        return NULL;
    }

    dev->port_letter = aPortChar;
    dev->fdev = ret;
    for(i=0; i < PORT_SIZE; i++)
    {
        dev->fpin[i] = -1;
        dev->irq_mode[i] = GPIO_IRQ_MODE_NOINT;
    }

    return dev;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_direction(struct as_gpio_device *aDev,
                                  int aPinNum,
                                  int aDirection)
{
    int ret=0;
    int portval;

    /* Set PIN as GPIO; read/modify/write */
    ret = ioctl(aDev->fdev, GPIORDMODE, &portval);
    if (ret < 0) {
        ERROR("Can't set gpio read mode\n");
        return ret;
    }

    portval |= (1 << aPinNum);

    ret = ioctl(aDev->fdev, GPIOWRMODE, &portval);
    if (ret < 0) {
        ERROR("Can't set gpio write mode\n");
        return ret;
    }

    /* set direction */
    ret = ioctl(aDev->fdev, GPIORDDIRECTION, &portval);
    if (ret < 0) {
        ERROR("Can't get gpio direction\n");
        return ret;
    }

    if (aDirection == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }

    ret = ioctl(aDev->fdev, GPIOWRDIRECTION, &portval);
    if (ret < 0) {
        ERROR("Can't get gpio direction\n");
        return ret;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_value(struct as_gpio_device *aDev,
                              int aPinNum,
                              int aValue)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDDATA, &portval);
    if (ret < 0) {
        ERROR("Can't read gpio data\n");
        return ret;
    }

    if (aValue == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }
    ret = ioctl(aDev->fdev, GPIOWRDATA, &portval);
    if (ret < 0) {
        ERROR("Can't write gpio data\n");
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_value(struct as_gpio_device *aDev,
                              int aPinNum)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDDATA, &portval);
    if (ret < 0) {
        ERROR("Can't read gpio data\n");
        return ret;
    }

    if ((portval & (1<<aPinNum)) != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_blocking_get_pin_value(struct as_gpio_device *aDev,
                                       int aPinNum,
                                       int aDelay_s,
                                       int aDelay_us)
{
    int ret;
    char value;

    fd_set rfds;
    struct timeval tv;
    int retval;


    if (aPinNum >= PORT_SIZE )
    {
        ERROR("Pin num %d upper than port size (%d)\n", aPinNum, PORT_SIZE);
        return -1;
    }
    if (aPinNum < 0 )
    {
        ERROR("Wrong pin number %d\n",aPinNum);
        return -1;
    }

    if (aDev->fpin[aPinNum] == -1)
    {
        ERROR("irq must be configured before\n");
        return -1;
    }

    FD_ZERO(&rfds);
    FD_SET(aDev->fpin[aPinNum], &rfds);

    tv.tv_sec = aDelay_s;
    tv.tv_usec = aDelay_us;

    /* flush the file */
    retval = select(aDev->fpin[aPinNum]+1, &rfds, NULL, NULL, &tv);
    if (retval > 0)
    {
        ret = read(aDev->fpin[aPinNum], &value, 1);
        if (ret < 0)
        {
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

int32_t as_gpio_get_pullup_value(struct as_gpio_device *aDev,
                                 int aPinNum)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDPULLUP, &portval);
    if (ret < 0) {
        ERROR("can't get pullup value\n");
        return ret;
    }

    if ((portval & (1<<aPinNum)) != 0) {
        return 1;
    } else {
        return 0;
    }

}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pullup_value(struct as_gpio_device *aDev,
                                 int aPinNum,
                                 int aValue)
{
    int ret=0;
    int portval;

    ret = ioctl(aDev->fdev, GPIORDPULLUP, &portval);
    if (ret < 0) {
        ERROR("Can't get pullup value\n");
        return ret;
    }

    if (aValue == 0) {
        portval &= ~(1 << aPinNum);
    } else {
        portval |= (1 << aPinNum);
    }
    ret = ioctl(aDev->fdev, GPIOWRPULLUP, &portval);
    if (ret < 0) {
        ERROR("Can't set pullup value\n");
        return ret;
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_irq_mode(struct as_gpio_device *aDev,
                             int aPinNum)
{
    int ret=0;
    int portval;

    /* check pin num */
    if (aPinNum >= PORT_SIZE)
    {
        ERROR("Pin num %d upper than port size (%d)\n", aPinNum, PORT_SIZE);
        return -1;
    }

    if (aPinNum < PORT_SIZE/2)
    {

        ret = ioctl(aDev->fdev, GPIORDIRQMODE_L, &portval);
        if (ret < 0) {
            ERROR("Can't read irq mode L\n");
            return ret;
        }

        portval &= (3 << (aPinNum * 2));
        return portval >> (aPinNum * 2);

    } else {

        ret = ioctl(aDev->fdev, GPIORDIRQMODE_H, &portval);
        if (ret < 0) {
            ERROR("Can't read irq mode H\n");
            return ret;
        }

        portval &= (3 << ((aPinNum - (PORT_SIZE/2)) * 2));
        return portval >> ((aPinNum - (PORT_SIZE/2)) * 2);
    }
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_irq_mode(struct as_gpio_device *aDev,
                             int aPinNum,
                             int aMode)
{
    int ret=0;
    int portval;
    char buffer[BUFF_SIZE];

    fd_set rfds;
    struct timeval tv;
    int retval;


    /* check mode value */
    if (aMode > 3)
    {
        ERROR("Wrong mode value %d\n",aMode);
        return -1;
    }

    /* check pin num */
    if (aPinNum >= PORT_SIZE)
    {
        ERROR("Pin num %d upper than port size (%d)\n", aPinNum, PORT_SIZE);
        return -2;
    }

    /* close fpin file */
    if (aDev->fpin[aPinNum] != -1)
    {
        close(aDev->fpin[aPinNum]);
        aDev->fpin[aPinNum] = -1;
    }

    if (aPinNum < PORT_SIZE/2)
    {
        ret = ioctl(aDev->fdev, GPIORDIRQMODE_L, &portval);
        if (ret < 0) {
            ERROR("Can't read irq mode L\n");
            return ret;
        }

        portval &= ~(3 << (aPinNum * 2));
        portval |=  (aMode << (aPinNum * 2));

        ret = ioctl(aDev->fdev, GPIOWRIRQMODE_L, &portval);
        if (ret < 0) {
            ERROR("Can't write irq mode L\n");
            return ret;
        }

    } else {

        ret = ioctl(aDev->fdev, GPIORDIRQMODE_H, &portval);
        if (ret < 0) {
            ERROR("Can't read irq mode H\n");
            return ret;
        }

        portval &= ~(3 << ((aPinNum - (PORT_SIZE/2)) * 2));
        portval |=  (aMode << ((aPinNum - (PORT_SIZE/2)) * 2));

        ret = ioctl(aDev->fdev, GPIOWRIRQMODE_H, &portval);
        if (ret < 0) {
            ERROR("Can't write irq mode H\n");
            return ret;
        }
    }

    if ((aMode != GPIO_IRQ_MODE_NOINT) && (aDev->fpin[aPinNum] == -1))
    {
        /* open fpin file */
        ret = snprintf(buffer, BUFF_SIZE, "%s%c%d",
                       GPIO_BASE_PIN, aDev->port_letter, aPinNum);
        if (ret < 0)
        {
            ERROR("Can't forge fpin path\n");
            return ret;
        }

        ret = open(buffer, O_RDONLY);
        if (ret < 0)
        {
            ERROR("Can't open %s\n",buffer);
            return ret;
        }
        aDev->fpin[aPinNum] = ret;

    }


    FD_ZERO(&rfds);
    FD_SET(aDev->fpin[aPinNum], &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1;


    /* flush the file */
    retval = select(aDev->fpin[aPinNum]+1, &rfds, NULL, NULL, &tv);
    if (retval)
    {
        read(aDev->fpin[aPinNum], buffer, BUFF_SIZE);
    }

    aDev->irq_mode[aPinNum] = aMode;
    return 0;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_close(struct as_gpio_device *aDev)
{
    int i;
    close(aDev->fdev);
    for (i=0; i < PORT_SIZE; i++)
    {
        if (aDev->fpin[i] != -1)
            close(aDev->fpin[i]);
    }
    free(aDev);
    return 0;
}

