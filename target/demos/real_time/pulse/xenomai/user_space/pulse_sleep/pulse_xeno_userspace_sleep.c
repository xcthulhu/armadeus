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

#include <signal.h>

#include <native/task.h>
#include <native/timer.h>
#include <rtdm/rtdm.h>
#include "../../../../common.h"

RT_TASK blink_task;
int write_fd = -1;

void blink(void *arg){
	int iomask;    
   	struct timespec tim = {0,TIMESLEEP};

  	iomask=0x00;

  	while(1){
    		write(write_fd,&iomask,sizeof(iomask));
    		iomask^=1;
    		if (nanosleep(&tim,NULL) != 0) {
		    	printf("erreur de usleep\n");
	    		return;
    		}
  	}
}


void catch_signal(int sig){}
 
int main(int argc, char **argv) {

	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);


	/* Avoid memory swapping for this program */
	mlockall(MCL_CURRENT|MCL_FUTURE);


	/* Ouverture du descripteur de fichier */
	if ((write_fd = open(PULSE_OUTPUT_DEV, O_WRONLY)) < 0) {
		printf("Cannot open /dev/%s\n",PULSE_OUTPUT_DEV);
		return 1;
	}

	rt_task_create(&blink_task, "blinkLed", 0, 99, 0);
	rt_task_start(&blink_task, &blink, NULL);
	
	pause();

	rt_task_delete(&blink_task);
	rt_dev_close(write_fd);
	return 0;
}
