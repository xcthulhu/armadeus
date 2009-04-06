/*
* Small xenomai userspace apps for generating signal
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>
#include "../../../../common.h"

RT_TASK blink_task;

void blink(void *arg){
	int fd, iomask;
  	if ((fd = open(PULSE_OUTPUT_DEV, O_RDWR))<0) {
    		printf("Open error on %s\n",PULSE_OUTPUT_DEV);
    		exit(0);
  	}
  	iomask=0x00;

  	rt_task_set_periodic(NULL, TM_NOW, TIMESLEEP*1000);
  
  	while(1){
    		rt_task_wait_period(NULL);
    		write(fd,&iomask,sizeof(iomask));
    		iomask^=1;
  	}
  	close(fd);
}

void catch_signal() {}

int main(int argc, char **argv) {

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);

	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);

	/*
	 * Arguments: &task,
	 *            name,
	 *            stack size (0=default),
	 *            priority,
	 *            mode (FPU, start suspended, ...)
	 */
	rt_task_create(&blink_task, "blinkLed", 0, 99, 0);

	/*
	 * Arguments: &task,
	 *            task function,
	 *            function argument
	 */
	rt_task_start(&blink_task, &blink, NULL);
	pause();
	rt_task_delete(&blink_task);
	return 0;
}
