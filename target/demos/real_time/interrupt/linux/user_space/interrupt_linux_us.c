/*
* Linux userspace app for handling "interrupts" on a GPIO device descriptor
* (with blocking read) and toggling an other GPIO at each occurancy.
*
* Copyright (C) 2009 Armadeus Systems / Armadeus Project
* Author: <gwenhael.goavec-merou@armadeus.com>
*
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "../../../common.h"


int main(void)
{
	int fd_input, fd_output, retval = 0, iomask=0x00;
	char c;

	/* Open GPIO device for communication */
  	fd_input = open (INTERRUPT_INPUT_DEV, O_RDONLY);
  	if (fd_input < 0) {
		printf ("Open Failed : %s\n", INTERRUPT_INPUT_DEV);
		exit (EXIT_FAILURE);
	}
	fd_output = open(INTERRUPT_OUTPUT_DEV, O_RDWR);
	if (fd_output < 0) {
		printf("Open Failed : %s\n", INTERRUPT_OUTPUT_DEV);
		exit (EXIT_FAILURE);
	}

	/* Set this process as owner of device file */
	retval = fcntl(fd_input, F_SETOWN, getpid());
  	if (retval < 0) {
		printf ("F_SETOWN fails for%s\n", INTERRUPT_INPUT_DEV);
		return EXIT_FAILURE;
	}
	retval = fcntl(fd_output, F_SETOWN, getpid());
	if (retval < 0) {
		printf("F_SETOWN fails for %s\n", INTERRUPT_OUTPUT_DEV);
		return EXIT_FAILURE;
	}

	while(1) {
		read (fd_input, &c, 1);
		write(fd_output, &iomask, sizeof(iomask));
		iomask ^=1;/* (iomask == 0x00)?0x01:0x00; */
	}
  	close(fd_input);
	close(fd_output);

  	exit (EXIT_SUCCESS);
}
