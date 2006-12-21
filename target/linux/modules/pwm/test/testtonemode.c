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

int main(void)
{
	int handle;
	short	ret;
	unsigned short data[]= {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

	handle = open("/dev/pwm", O_RDWR);
	if(handle == 0){
		printf("can not open file pwm\n");
		return -1;
	}

	printf("Play tone file...\n");

	// Play tone
	ioctl(handle, PWM_IOC_SMODE, PWM_TONE_MODE);
	ioctl(handle, PWM_IOC_SPERIOD, 250);	// period in ms
	ioctl(handle, PWM_IOC_SSAMPLE, 0x75); // Sample register

	write(handle, data, sizeof(data));

	close(handle);

	return 1;
}

