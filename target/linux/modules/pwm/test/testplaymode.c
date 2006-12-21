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
#include <fcntl.h>
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

int main(void)
{
	int handle;
	short	ret;

	handle = open("/dev/pwm", O_RDWR);
	if(handle == 0){
		printf("can not open file pwm\n");
		return -1;
	}

	printf("Play wave file...\n");

	// Play wave
	ioctl(handle, PWM_IOC_SMODE, PWM_PLAY_MODE);
	ioctl(handle, PWM_IOC_SFREQ, SAM_RATE);
	ioctl(handle, PWM_IOC_SDATALEN, DATA_LEN);

	write(handle, data, DATA_SZ);

	close(handle);
	
	return 1;
}

