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

#include "as_spi.h"

int as_spi_open(const unsigned char *spidev_name)
{
    /* TODO */
    return -1;
}

int as_spi_set_mode(int fd, uint8_t mode)
{
    /* TODO */
    return -1;
}

int as_spi_set_lsb(int fd, uint8_t lsb)
{
    /* TODO */
    return -1;
}

int as_spi_set_bits_per_word(int fd, uint8_t bits)
{
    /* TODO */
    return -1;
}

int as_spi_get_mode(int fd)
{
    /* TODO */
    return -1;
}

int as_spi_get_lsb(int fd)
{
    /* TODO */
    return -1;
}

int as_spi_get_bits_per_word(int fd)
{
    /* TODO */
    return -1;
}

uint32_t as_spi_forge_msg(int fd, 
                          uint32_t msg, 
                          size_t len,
                          uint32_t speed)
{
    /* TODO */
    return -1;
}

void as_spi_close(int fd)
{
    /* TODO */
    return;
}



