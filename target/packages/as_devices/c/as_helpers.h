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

#ifndef __ASHELPERS_H__
#define __ASHELPERS_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#   define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#   define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#endif

/** @brief Write an int value as string in a /sys/ or /proc interface
 *
 * @param fd: file handler
 * @param value: int value to write
 *
 * @return number of chars written, negative value on error
 */
int as_write_buffer(int fd, int value);

/** @brief Write an int value as string in a /sys/ or /proc interface
 *
 * @param fd: file handler
 * @param string: char pointer to string to write
 *
 * @return number of chars written, negative value on error
 */
int as_write_buffer_string(int fd, char *string);

/** @brief read a string value in file
 *
 * @param fd: file handler
 * @param buf: char pointer to read buffer
 * @param size: size of buffer
 *
 * @return number of chars read, negative value on error
 */
int as_read_buffer(int fd, char *buf, int size);

/** @brief read an int value stored as string in a /sys/ or /proc interface
 *
 * @param fd: file handler
 * @param value_res: int pointer to store the value read
 *
 * @return negative value on error
 */
int as_read_int(int fd, int *value_res);

/** @brief return APF platform type
 *
 * @return APF_UNKNOWN/APF9328/APF27/APF51
 */
int as_helpers_get_platform(void);

enum {
	APF_UNKNOWN,
	APF9328,
	APF27,
	APF51,
};

#endif /* __ASHELPERS_H__ */

