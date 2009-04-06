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
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../../../common.h"
int fd;
int iomask=0x00;

void test(int signum) {
	write(fd, &iomask, sizeof(iomask));
	iomask ^=1;
}

int main(int argc, char **argv) { 	
	struct sigaction sa;
	struct itimerval timer;

	fd = open(PULSE_OUTPUT_DEV,O_RDWR);
	if (fd < 0) {
		printf("open : error\n");
		return 1;
	}
	
	/* Install test as the signal handler for SIGVTALRM */
	memset(&sa, 0,sizeof(sa));
	sa.sa_handler = &test;
	sigaction(SIGVTALRM,&sa, NULL);
	/* Configure the timer to expire after TIMESLEEP msec ... */
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = TIMESLEEP;
	/* ... and every TIMESLEEP msec after that. */
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = TIMESLEEP;
	/* Start a virtual timer. It counts down whenever this process is
	 * executing */
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
	while(1);
    return 0;
}
