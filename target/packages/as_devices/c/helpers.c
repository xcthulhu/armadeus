/*
** AsDevices - Helpers functions for other modules
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  /* for open()   */
#include <unistd.h> /* for write()  */

/* #define DEBUG */
#include "helpers.h"

#define SIZE_OF_BUFF 50


int as_write_buffer(int fd, int value)
{
    int ret;
    char buffer[16];
    int buffer_len;

    ret = snprintf(buffer, 16, "%d", value);
    if (ret < 0)
        return ret;

    buffer_len = strlen(buffer);
    ret = write(fd, buffer, buffer_len);
    if (ret < 0) {
        ERROR("Can't write file ");
        return ret;
    }
    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        ERROR("lseek error ");
        return ret;
    }

    return buffer_len;
}

int readBuffer(int fd, char *buf)
{
    int ret;

    ret = read(fd, buf, SIZE_OF_BUFF);
    if (ret < 0) {
        ERROR("Can't read file ");
        return ret;
    }
    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        ERROR("lseek error ");
        return ret;
    }

    return ret;
}

