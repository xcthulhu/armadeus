/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2002 Motorola Semiconductors HK Ltd
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <errno.h>
//#include "MX1_def.h"
#include "../pwm.h"

#define	T8_8
//#define	T16_8
//#define	T32_8

#ifdef T8_8
#include "TEST8.H"
#define SAM_RATE	PWM_SAMPLING_8KHZ
#define DATA_LEN	PWM_DATA_8BIT
#define DATA_SZ		0xd998
#elif defined(T16_8)
#include "T16K_8B.H"
#define SAM_RATE	PWM_SAMPLING_16KHZ
#define DATA_LEN	PWM_DATA_8BIT
#define DATA_SZ		0x7520
#elif defined(T32_8)
#include "T32K_8B.H"
#define SAM_RATE	PWM_SAMPLING_32KHZ
#define DATA_LEN	PWM_DATA_8BIT
#define DATA_SZ		0x7520
#endif

#define BUFFER_SIZE (16*1024)


int main( int argc, char* argv[] )
{
    int handle, readfd = 0, nb = 0, written = 0;
    short	ret;
    unsigned char* buffer;

    if (argc != 1) {
        printf("Opening file: %s\n", argv[1]);
        readfd = open( argv[1], O_RDONLY );
    }

    handle = open("/dev/pwm", O_RDWR);
    if (handle < 0) {
        printf("can not open file /dev/pwm\n");
        exit(1);
    }

    printf("Playing wave file...\n");

    /* Play wave */
    ioctl(handle, PWM_IOC_SMODE, PWM_PLAY_MODE);
    ioctl(handle, PWM_IOC_SFREQ, SAM_RATE);
    ioctl(handle, PWM_IOC_SDATALEN, DATA_LEN);

    if (readfd) {
        buffer = (unsigned char*)malloc( BUFFER_SIZE );
        if (buffer) {
            while ( (nb = read( readfd, buffer, BUFFER_SIZE )) > 0 ) {
                write( handle, buffer, nb );
                printf("read %d\n", nb);
            }
        }
    } else {
        printf("No data file given, playing default sound of size %d\n", DATA_SZ);
        buffer = (unsigned char*)data;
        while (nb < DATA_SZ) {
            written = write(handle, buffer, DATA_SZ);
            buffer += written;
            nb += written;
            printf("  Written %d, total %d\n", written, nb);
        }
        buffer = 0;
    }

    printf("End of sound playing\n");
    close(handle);
    close(readfd);
    if (buffer)
        free( buffer );

    exit(0);
}

