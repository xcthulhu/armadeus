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
 * Copyright (C) 2009 Fabien Marteau <fabien.marteau@armadeus.com> 
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "as_spi.h"


int as_spi_open(const unsigned char *aSpidev_name)
{
    int fd;

    fd = open((char *)aSpidev_name, O_RDWR);
    if (fd < 0) {
            perror("open");
            return -1;
    }

    return fd;
}

int as_spi_set_mode(int aFd, uint8_t aMode)
{
    /* TODO */
    return -1;
}

int as_spi_set_lsb(int aFd, uint8_t aLsb)
{
    /* TODO */
    return -1;
}

int as_spi_set_bits_per_word(int aFd, uint8_t aBits)
{
    /* TODO */
    return -1;
}

int as_spi_set_speed(int aFd)
{
    /* TODO */
    return -1;
}

int as_spi_get_mode(int aFd)
{
    uint8_t mode;

    if (ioctl(aFd, SPI_IOC_RD_MODE, &mode) < 0) {
            perror("SPI rd_mode");
            return -1;
    }

    return mode;
}

int as_spi_get_lsb(int aFd)
{
    uint8_t lsb;

    if (ioctl(aFd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
            perror("SPI rd_lsb_fist");
            return -1;
    }

    return lsb;
}

int as_spi_get_bits_per_word(int aFd)
{
    uint8_t bits;

    if (ioctl(aFd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
            perror("SPI bits_per_word");
            return -1;
    }

    return bits;
}

int as_spi_get_speed(int aFd)
{
    uint8_t speed;

    if (ioctl(aFd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
            perror("SPI max_speed_hz");
            return -1;
    }

    return speed;
}

uint32_t as_spi_msg(int aFd, 
                    uint32_t aMsg, 
                    size_t aLen,
                    uint32_t aSpeed)
{
    uint32_t msg;
    int len;

    struct spi_ioc_transfer xfer[1];
    unsigned char buf[32];
    unsigned char buf_read[32];

    int status;
    int i;

    msg = aMsg;
    len = aLen;

    msg = msg << 1;/* XXX  Last bit is always read at 0 */
    len = len + 1; /* XXX */

    memset(xfer, 0, sizeof(xfer));
    memset(buf, 0, sizeof(buf));
    memset(buf_read, 0, sizeof(buf_read));

    if (len >= sizeof(buf)) {
        len = sizeof(buf) - 1;
    }

    for (i=len; i>0; i--)
    {
        buf[i-1] = msg & 0x01;
        msg = msg >> 1;
    }

    xfer[0].tx_buf = (uint32_t)buf;
    xfer[0].len = len;
    xfer[0].rx_buf = (uint32_t)buf_read;
    xfer[0].speed_hz = aSpeed;
    xfer[0].bits_per_word = 1;

    status = ioctl(aFd, SPI_IOC_MESSAGE(1), xfer);
    if (status < 0) {
        perror("SPI_IOC_MESSAGE");
        return 0;
    }

    msg = msg | buf_read[0];
    for (i=1; i<len; i++)
    {
        msg = msg << 1;
        msg = msg | buf_read[i];
    }
    return msg; 
}

void as_spi_close(int aFd)
{
    close(aFd);
    return;
}

