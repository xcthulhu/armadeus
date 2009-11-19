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

#include "as_93lcxx.h"

struct as_93lcxx_device * as_93lcxx_open(unsigned char *spidev_filename,
                                         uint8_t type,
                                         uint32_t speed,
                                         uint8_t word_size)
{
    /* TODO */
    return NULL;
}

int as_93lcxx_read(struct as_93lcxx_device *dev, uint16_t address)
{
    /* TODO */
    return -1;
}

int as_93lcxx_ewen(struct as_93lcxx_device *dev)
{
    /* TODO */
    return -1;
}

int as_93lcxx_erase(struct as_93lcxx_device *dev, uint16_t address)
{
    /* TODO */
    return -1;
}

int as_93lcxx_erase_all(struct as_93lcxx_device *dev)
{
    /* TODO */
    return -1;
}

int as_93lcxx_write(struct as_93lcxx_device *dev, 
                    uint16_t address, 
                    uint16_t value)
{
    /* TODO */
    return -1;
}

int as_93lcxx_write_all(struct as_93lcxx_device *dev, 
                        uint16_t value)
{
    /* TODO */
    return -1;
}

int as_93lcxx_ewds(struct as_93lcxx_device *dev)
{
    /* TODO */
    return -1;
}


