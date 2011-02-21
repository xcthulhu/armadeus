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
 * Copyright (C) 2011 Fabien Marteau <fabien.marteau@armadeus.com>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */
#include <string.h>

#include "as_dac.h"
#include "as_max5821.h"

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

struct as_dac_device *as_dac_open(const char *aDacType, int aBusNumber,
                                  int aAddress, int aVRef)
{
    if (strncmp(AS_MAX5821_TYPE, aDacType, strlen(AS_MAX5821_TYPE)) == 0)
        return as_dac_open_max5821(aBusNumber, aAddress, aVRef);
    /* TODO: mcp4912 */
    ERROR("Undefined DAC type '%s'\n", aDacType);
    return NULL;
}

int32_t as_dac_set_value_in_millivolts(struct as_dac_device *aDev,
                                       int aChannel,
                                       int aValue)
{
    if (strncmp(AS_MAX5821_TYPE, aDev->device_type, strlen(AS_MAX5821_TYPE)) == 0)
        return as_dac_set_value_max5821(aDev, aChannel, aValue);

    ERROR("Undefined DAC type '%s'\n", aDev->device_type);
    return -1;
}


int32_t as_dac_close(struct as_dac_device *aDev)
{
    if (strncmp(AS_MAX5821_TYPE, aDev->device_type, strlen(AS_MAX5821_TYPE)) == 0)
        as_dac_close_max5821(aDev);

    free(aDev);
    return 0;
}
