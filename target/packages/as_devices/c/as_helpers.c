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
#include "as_helpers.h"

#define SIZE_OF_BUFF 50

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)


int as_write_buffer(int fd, int value)
{
    int ret;
    char buffer[16];
    int buffer_len;

    ret = snprintf(buffer, 16, "%d\n", value);
    if (ret < 0)
        return ret;

    buffer_len = strlen(buffer);
    ret = write(fd, buffer, buffer_len);
//XXX    if (ret < 0) {
//XXX        ERROR("Can't write file, buffer lenght %d $>%s<$\n", buffer_len, buffer);
//XXX        return ret;
//XXX    }
    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        ERROR("lseek error\n");
        return ret;
    }

    return buffer_len;
}

int as_write_buffer_string(int fd, char *string)
{
    int ret;
    char buffer[SIZE_OF_BUFF];
    int string_len;

    string_len = strlen(string);
    ret = write(fd, string, string_len);
    if (ret < 0) {
        ERROR("Can't write file ");
        return ret;
    }
    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        ERROR("lseek error ");
        return ret;
    }

    return string_len;
}

int as_read_buffer(int fd, char *buf, int size)
{
    int ret, err;

    ret = read(fd, buf, size);
    if (ret < 0) {
        ERROR("Can't read file ");
        return ret;
    }
    err = lseek(fd, 0, SEEK_SET);
    if (err < 0) {
        ERROR("lseek error ");
        return err;
    }

    return ret;
}

int as_read_int(int fd, int *value_res)
{
    const int SIZEOFBUFF = 20;
    ssize_t ret;
    char buf[SIZEOFBUFF];
    int value;

    ret = as_read_buffer(fd, buf, SIZEOFBUFF);
    if (ret < 0) return ret;
    buf[ret-1] = '\0';
    value = strtol(buf, NULL, 10);
    *value_res = value;

    return 0;
}

