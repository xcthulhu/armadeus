/*
* Small xenomai user space for handling interrupt 
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

#include <sys/mman.h>
#include <native/task.h>
#include <native/intr.h>
#include "../../../common.h"

RT_INTR intr_desc;

RT_TASK server_desc;
void cleanup(void);

void irq_server (void *cookie) {
	int fd ,iomask=0x00;
	if (rt_intr_enable(&intr_desc) != 0) {
		printf("Error : enabling IT\n");
		return;
	}
	if ((fd = open(INTERRUPT_OUTPUT_DEV, O_RDWR))<0) {
		printf("Open error on /dev/gpio/portD\n");
		return;
	}

	for (;;) {
    		/* Wait for the next interrupt. */
    		if (rt_intr_wait(&intr_desc,TM_INFINITE) > 0) {
			write(fd,&iomask,sizeof(iomask));
			iomask^=1;
    		}
  	}
}

int main (int argc, char *argv[]) {
	int err;

	mlockall(MCL_CURRENT|MCL_FUTURE);
  
  	/* Version With 4 param only on userSpace */
	err = rt_intr_create(&intr_desc,"MyIrq",70,0);
  	if (err != 0){
    		printf("rt_intr_create : error\n");
    		return 1;
	}

	err = rt_task_create(&server_desc,
		"MyIrqServer",
		TASK_STKSZ,
		TASK_PRIO,
		TASK_MODE);
  	if (err == 0)
    		rt_task_start(&server_desc,&irq_server,NULL);
  	else{
    		printf("rt_task_start : error\n");
    		return 1;
  	}
  	getchar();
  	cleanup();
  	return 0;
}

void cleanup (void) {
	rt_intr_delete(&intr_desc);
	rt_task_delete(&server_desc);
}
