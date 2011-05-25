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
 * Copyright (C) 2009  Benoît Ryder <benoit@ryder.fr>
 * Copyright (C) 2010, 2011  Fabien Marteau <fabien.marteau@armadeus.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "as_i2c.h"

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

/* Format of path to I2C devices */
#define AS_I2C_DEV_PATH_FMT "/dev/i2c-%u"
#define _STR(x) #x
#define AS_I2C_DEV_PATH_SIZE (sizeof(AS_I2C_DEV_PATH_FMT)-2+sizeof(_STR(AS_I2C_DEV_COUNT))-1)

struct as_i2c_device *as_i2c_open(int aBusNumber)
{
    char buf[AS_I2C_DEV_PATH_SIZE];
    int fdev;
    struct as_i2c_device *dev;

    snprintf(buf, sizeof(buf), AS_I2C_DEV_PATH_FMT, aBusNumber);

    fdev = open(buf, O_RDWR);
    if (fdev < 0) {
        ERROR("Can't open i2c bus /dev/i2c-%d\n", aBusNumber);
        return NULL;
    }

    dev = malloc(sizeof(struct as_i2c_device));
    if (dev == NULL) {
        ERROR("Can't allocate memory for device structure\n");
        return NULL;
    }

    dev->i2c_id = aBusNumber;
    dev->slave_addr = 0;
    dev->fi2c = fdev;

    return dev;
}

int32_t as_i2c_close(struct as_i2c_device *aDev)
{
    close(aDev->fi2c);
    free(aDev);

    return 0;
}

int32_t as_i2c_set_slave_addr(struct as_i2c_device *aDev, uint8_t aAddr)
{
    if (aAddr == 0) {
        ERROR("Wrong slave address\n");
        return -1;
    }
    if (ioctl(aDev->fi2c, I2C_SLAVE_FORCE, aAddr) < 0) {
        ERROR("Can't set slave address\n");
        return -1;
    }
    aDev->slave_addr = aAddr;

    return 0;
}

int32_t as_i2c_get_slave_addr(struct as_i2c_device *aDev) {
    return aDev->slave_addr;
}

int32_t as_i2c_read(struct as_i2c_device *aDev,
                    uint8_t *aData, size_t n)
{
    struct i2c_msg msg = { aDev->slave_addr, I2C_M_RD, n, aData };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }
    if (n <= 0) {
        ERROR("Size of data must be upper than 0\n");
        return -1;
    }

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0)
        return -1;

    return 0;
}

int32_t as_i2c_write(struct as_i2c_device *aDev,
                     uint8_t *aData, size_t n)
{
    struct i2c_msg msg = { aDev->slave_addr, 0, n, (uint8_t *)aData };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }
    if (n <= 0) {
        ERROR("Size of data must be upper than 0\n");
        return -1;
    }

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0)
        return -1;

    return 0;
}

int32_t as_i2c_read_reg(struct as_i2c_device *aDev,
                        uint8_t aReg, uint8_t *aData, size_t n)
{
    /* write reg */
    struct i2c_msg msg = { aDev->slave_addr, 0, 1, &aReg };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }

    if (n <= 0) {
        ERROR("Size of data must be upper than 0\n");
        return -1;
    }

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0) {
        ERROR("Can't write on i2c\n");
        return -1;
    }
    /* read data */
    msg.flags = I2C_M_RD;
    msg.len = n;
    msg.buf = aData;

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0) {
        ERROR("Can't read on i2c\n");
        return -2;
    }

    return 0;
}

int32_t as_i2c_write_reg(struct as_i2c_device *aDev,
                         uint8_t aReg, const uint8_t *aData, size_t n)
{
    uint8_t buf[n+1];

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }
    if (n <= 0) {
        ERROR("Size of data must be upper than 0\n");
        return -1;
    }

    buf[0] = aReg;
    memcpy(buf+1, aData, n);

    return as_i2c_write(aDev, buf, sizeof(buf));
}

int32_t as_i2c_read_reg_byte(struct as_i2c_device *aDev, uint8_t aReg)
{
    uint8_t val;
    int ret;

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }
    ret = as_i2c_read_reg(aDev, aReg, &val, 1);
    if (ret < 0)
        return ret;

    return val;
}

int32_t as_i2c_write_reg_byte(struct as_i2c_device *aDev,
                              uint8_t aReg, uint8_t aVal)
{
    uint8_t buf[2] = { aReg, aVal };

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }

    return as_i2c_write(aDev, buf, 2);
}

int32_t as_i2c_read_msg(struct as_i2c_device *aDev,
                          uint8_t *aWData, uint8_t aWriteSize,
                          uint8_t *aRData, size_t aReadSize)
{
    /* write reg */
    struct i2c_msg msg = { aDev->slave_addr, 0, aWriteSize, aWData };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (aDev->slave_addr == 0) {
        ERROR("Slave address must be set before\n");
        return -1;
    }

    if (aWriteSize <= 0) {
        ERROR("Write size must be upper than 0\n");
        return -1;
    }
    if (aReadSize <= 0) {
        ERROR("Read size must be upper than 0\n");
        return -1;
    }

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0) {
        ERROR("Can't write on i2c\n");
        return -1;
    }
    /* read data */
    msg.flags = I2C_M_RD;
    msg.len = aReadSize;
    msg.buf = aRData;

    if (ioctl(aDev->fi2c, I2C_RDWR, &rdwr) < 0) {
        ERROR("Can't read on i2c\n");
        return -2;
    }

    return 0;
}


