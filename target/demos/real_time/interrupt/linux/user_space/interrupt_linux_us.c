/*
* linux userspace apps for handling interrupts
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>


#define FALSE 0
#define TRUE 1
volatile int STOP=FALSE;
int wait_flag=TRUE;
void
action (int signo, siginfo_t * si, void *data)
{
  printf (" -------------- Got the %d signal ------------\n", signo);
}

int
main (int argc, char **argv)
{
  int fd, res, oflags, retval = 0;
  unsigned int buffer;
char buf[255];

  // Open gpio file device for communication;
  fd = open ("/dev/gpio/PA6", O_RDONLY);
  if (fd < 0)
    {
      printf ("Open Failed\n");
      exit (EXIT_FAILURE);
    }

  // set this process as owner of device file
  retval = fcntl (fd, F_SETOWN, getpid ());
  if (retval < 0)
    {
      printf ("F_SETOWN fails \n");
    }
  while (STOP == FALSE)
    {
      usleep (100000);
	  res = read (fd, buf, 255);
	printf(".\n");
	  buf[res] = 0;
	  printf ("%d:%x:%d\n", (int)buf[6],buf, res);
    }
  close (fd);
  exit (EXIT_SUCCESS);
}
