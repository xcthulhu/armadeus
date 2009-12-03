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
#include "as_spi.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* define start bits + opcode */
#define READ      (6)
#define EWEN      (4)
#define EWEN_ADDR (3) 
#define ERASE     (7)
#define ERAL      (4)
#define ERAL_ADDR (2)
#define WRITE     (5)
#define WRAL      (4)
#define WRAL_ADDR (1)
#define EWDS      (4)
#define EWDS_ADDR (0)

/* Define timings */
#define MS  (1000) 
#define TWC (6*MS)
#define TEC (6*MS)
#define TWL (15*MS)

/** @brief Private functions to ease making spi instructions
 */

uint8_t address_len(uint8_t aType, uint8_t aWord_size)
{
    switch(aType)
    {
        case 46:
            if (aWord_size == 16)
                return 6;
            else
                return 7;
        case 56:
        case 66:
            if (aWord_size == 16)
                return 8;
            else
                return 9;
        default:
            return 0;
    }
}

struct as_93lcxx_device * as_93lcxx_open(unsigned char *aSpidev_filename,
                                          uint8_t aType,
                                          uint32_t aSpeed,
                                          uint8_t aWord_size)
{
    int fd=-1;
    struct as_93lcxx_device *dev;

    /* open spidev bus */
    fd = as_spi_open(aSpidev_filename);
    if (fd < 0)
        return NULL;
    
    /* verify datas */
    if (!( (aType == 46) || (aType == 56) || (aType == 66)))
        return NULL;
    if ( (aWord_size < 1) || (aWord_size > 16))
        return NULL;

    dev = (struct as_93lcxx_device *)malloc(sizeof(struct as_93lcxx_device));
    if (dev == NULL)
        return NULL;

    /* fill in spidev structure */
    dev->spidev_filename = aSpidev_filename;
    dev->type = aType;
    dev->speed = aSpeed;
    dev->word_size = aWord_size;
    dev->fd = fd;
    
    return dev;
}

void as_93lcxx_close(struct as_93lcxx_device *aDev)
{
    as_spi_close(aDev->fd);
    free(aDev);
}

int32_t as_93lcxx_read(struct as_93lcxx_device *aDev, uint16_t aAddress)
{
    uint32_t data_out,msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((READ << (add_length+ (aDev->word_size)))
            | (aAddress << (aDev->word_size)));

    data_out = as_spi_msg(aDev->fd, msg,
                          3 + add_length + aDev->word_size,
                          aDev->speed);

    if (aDev->word_size == 8)
        return data_out & 0xff;
    else
        return data_out & 0xffff;
}

int32_t as_93lcxx_ewen(struct as_93lcxx_device *aDev)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((EWEN << add_length)
          |(EWEN_ADDR << (add_length-2))
           );

    as_spi_msg(aDev->fd, msg,
               3 + add_length,
               aDev->speed);
    return 0;
}

int32_t as_93lcxx_erase(struct as_93lcxx_device *aDev, uint16_t aAddress)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((ERASE << (add_length + aDev->word_size))
            | (aAddress << (aDev->word_size)));

    as_spi_msg(aDev->fd, msg,
               3 + add_length + aDev->word_size,
               aDev->speed);

    usleep(TWC);

    return 0;
}

int32_t as_93lcxx_erase_all(struct as_93lcxx_device *aDev)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((ERAL << add_length)
          |(ERAL_ADDR << (add_length-2))
           );

    as_spi_msg(aDev->fd, msg,
               3 + add_length,
               aDev->speed);

    /* wait for end of erase all */
    usleep(TEC);

    return 0;
}

int32_t as_93lcxx_write(struct as_93lcxx_device *aDev, 
                        uint16_t aAddress, 
                        uint16_t aValue)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((WRITE << (add_length+ (aDev->word_size)))
            | (aAddress << (aDev->word_size))
            | aValue
           );

    as_spi_msg(aDev->fd, msg,
               3 + add_length + aDev->word_size,
               aDev->speed);

    /* Wait for end of writing */
    usleep(TWC);

    return 0;
}

int32_t as_93lcxx_write_all(struct as_93lcxx_device *aDev, 
                            uint16_t aValue)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((WRAL << (add_length+ (aDev->word_size)))
          |(WRAL_ADDR << ((aDev->word_size) + (add_length-2)))
          |aValue
           );

    as_spi_msg(aDev->fd, msg,
               3 + add_length + aDev->word_size,
               aDev->speed);

    /* Wait for end of write all */
    usleep(TWL);

    return 0;
}

int32_t as_93lcxx_ewds(struct as_93lcxx_device *aDev)
{
    uint32_t msg=0;
    int add_length;

    add_length = address_len(aDev->type,aDev->word_size);

    /* forge message */
    msg = ((EWDS << add_length)
          |(EWDS_ADDR << (add_length-2))
           );

    as_spi_msg(aDev->fd, msg,
               3 + add_length,
               aDev->speed);
    return 0;
}


