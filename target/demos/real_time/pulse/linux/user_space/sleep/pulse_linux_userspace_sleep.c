/*
* linux user space apps for generating signl with sleep
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

int main(int argc, char **argv) {

  int fd;
  int iomask;    
  if ((fd = open("/dev/gpio/PA4", O_RDWR))<0) {
    printf("Open error on /dev/gpio/PA4\n");
    exit(0);
  }
  iomask=0x00;
  printf("Opened on /dev/gpio/PA4\n");
  while(1){
    iomask^=1;
    write(fd,&iomask,sizeof(iomask));
    usleep(10000);
  }
  close(fd);
  exit(0);
}
