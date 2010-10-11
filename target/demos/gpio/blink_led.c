/*
 * Small program to show how to use IOCTL with GPIO driver
 *
 * Copyright (C) 2009 Julien Boibessot <julien.boibessot@armadeus.com>
 *                    Inspired by original example from S. Falck
 *                    Armadeus Project / Armadeus Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/ppdev.h> 

#ifdef apf9328
#define LED_GPIO_PIN	"/dev/gpio/PD31"
#elif apf27
#define LED_GPIO_PIN   "/dev/gpio/PF14"
#else
#error Board not defined
#endif

#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int) 
#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int) 

int main(int argc, char **argv)
{
	int fd;
	int i;
	int portval;
	unsigned int count = 10;   

	if (argc == 2) {
		count = atoi(argv[1]);
	}

	printf("Toggling LED %d times \n", count);
	
	printf("Opening %s\n", LED_GPIO_PIN);
	if ((fd = open(LED_GPIO_PIN, O_RDWR)) < 0) {
		perror("Error");
		exit(1);
	}

	/* Set LED PIN as GPIO; read/modify/write */
	portval = 1;
	ioctl(fd, GPIOWRMODE, &portval);

	/* Set LED GPIO as output; read/modify/write */
	ioctl(fd, GPIOWRDIRECTION, &portval);

	/* Blink the LED */
	for (i = 0; i < count; i++) {
		printf("Led ON\n"); /* pin <- 0 */
		ioctl(fd, GPIORDDATA, &portval);
		portval &= ~1;
		ioctl(fd, GPIOWRDATA, &portval);

		sleep(1);

		printf("Led OFF\n"); /* pin <- 1 */
		ioctl(fd, GPIORDDATA, &portval);
		portval |= 1;
		ioctl(fd, GPIOWRDATA, &portval);

		sleep(1);
	}

	close(fd);
	exit(0);
}
