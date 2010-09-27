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
 *
 */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "as_i2c.h"


/* Format of path to I2C devices */
#define AS_I2C_DEV_PATH_FMT "/dev/i2c-%u"
#define _STR(x) #x
#define AS_I2C_DEV_PATH_SIZE (sizeof(AS_I2C_DEV_PATH_FMT)-2+sizeof(_STR(AS_I2C_DEV_COUNT))-1)


int as_i2c_open(unsigned int i2c_id)
{
    char buf[AS_I2C_DEV_PATH_SIZE];

    snprintf(buf, sizeof(buf), AS_I2C_DEV_PATH_FMT, i2c_id);

    return open(buf, O_RDWR);
}

int as_i2c_close(int fd)
{
    return close(fd);
}

int as_i2c_set_slave(int fd, uint8_t addr)
{
    if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
        return -1;

    return 0;
}


int as_i2c_read(int fd, uint8_t addr, uint8_t *data, size_t n)
{
    struct i2c_msg msg = { addr, I2C_M_RD, n, data };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (ioctl(fd, I2C_RDWR, &rdwr) < 0)
        return -1;

    return 0;
}

int as_i2c_write(int fd, uint8_t addr, const uint8_t *data, size_t n)
{
    struct i2c_msg msg = { addr, 0, n, (uint8_t *)data };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (ioctl(fd, I2C_RDWR, &rdwr) < 0)
        return -1;

    return 0;
}


int as_i2c_read_reg(int fd, uint8_t addr, uint8_t reg, uint8_t *data, size_t n)
{
    /* write reg */
    struct i2c_msg msg = { addr, 0, 1, &reg };
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    if (ioctl(fd, I2C_RDWR, &rdwr) < 0)
        return -1;
    /* read data */
    msg.flags = I2C_M_RD;
    msg.len = n;
    msg.buf = data;
    if (ioctl(fd, I2C_RDWR, &rdwr) < 0)
        return -2;

    return 0;
}

int as_i2c_write_reg(int fd, uint8_t addr, uint8_t reg, const uint8_t *data, size_t n)
{
    uint8_t buf[n+1];

    buf[0] = reg;
    memcpy(buf+1, data, n);

    return as_i2c_write(fd, addr, buf, sizeof(buf));
}


int as_i2c_read_reg_byte(int fd, uint8_t addr, uint8_t reg)
{
    uint8_t val;
    int ret = as_i2c_read_reg(fd, addr, reg, &val, 1);

    if (ret < 0)
        return ret;

    return val;
}

int as_i2c_write_reg_byte(int fd, uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };

    return as_i2c_write(fd, addr, buf, 2);
}
