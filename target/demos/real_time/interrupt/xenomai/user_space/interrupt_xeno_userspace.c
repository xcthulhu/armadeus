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

#define IRQ_NUMBER 70  /* Intercept interrupt #7 */
#define TASK_PRIO  99 /* Highest RT priority */
#define TASK_MODE  0  /* No flags */
#define TASK_STKSZ 0  /* Stack size (use default one) */

RT_INTR intr_desc;

RT_TASK server_desc;
void cleanup(void);

void irq_server (void *cookie) {
  int err;
  if (rt_intr_enable(&intr_desc) != 0) {
    printf("erreur pour enable\n");
    return;
  }
  for (;;) {
    /* Wait for the next interrupt on channel #7. */
    err = rt_intr_wait(&intr_desc,TM_INFINITE);
	printf("tata plop\n");    
    if (err > 0) {
      printf("interrupt retrouvee %d\n",err);
    }
  }
}

int main (int argc, char *argv[]) {
  int err;
  
  mlockall(MCL_CURRENT|MCL_FUTURE);
  
  /* Version With 4 param only on userSpace */
  err = rt_intr_create(&intr_desc,"MyIrq",IRQ_NUMBER,0);
  if (err != 0){
    printf("rt_intr_create : error\n");
    return 1;
  } else
    printf("rt_intr_create : ok\n");
 
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
