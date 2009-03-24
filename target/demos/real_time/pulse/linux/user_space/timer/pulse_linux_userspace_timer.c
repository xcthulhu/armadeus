/*
* linux user space apps for generating signl with timer
*
* Copyright (C) 2009 <gwenhael.goavec-merou@armadeus.com>
*                         Armadeus Project / Armadeus Systems
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
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TIMESLEEP 1
#define PORT "/dev/gpio/PA4"

int fd;
int iomask;
void test() {
	printf("hello world\n");
	write(fd, &iomask, sizeof(iomask));
	iomask ^=1;
}

int main(int argc, char **argv) { 	
	fd = open(PORT,O_RDWR);
	if (fd < 0) {
		printf("open : error\n");
		return 1;
	}
	iomask = 1;
	do {
		signal(SIGALRM,test);
		alarm(TIMESLEEP);
		pause();
	} while(1);
    return 0;
}
