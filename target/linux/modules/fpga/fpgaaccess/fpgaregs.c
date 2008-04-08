/************************************************************************
 * a program to write/read values on fpga address map
 * 7 april 2008
 * fpgaregs.c
 *
 * (c) Copyright 2008    Armadeus project
 * Fabien Marteau <fabien.marteau@armadeus.com>
 *
 * A simple driver for reading and writing on
 * fpga throught a character file /dev/fpgaaccess
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
 **********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>

/* file management */
#include <sys/stat.h>
#include <fcntl.h>

/* as name said */
#include <signal.h>

/* sleep, write(), read() */
#include <unistd.h>

/* converting string */
#include <string.h>

int ffpga;

int main(int argc, char *argv[])
{
  unsigned short address;
  unsigned short value;
  int retval;

  printf( "Testing button driver\n" );

  if((argc < 2) || (argc > 3)){
    perror("invalid arguments number\nfpgaregs fpga_reg_add [value]\n");
    exit(EXIT_FAILURE);
  }

  ffpga=open("/dev/fpgaaccess",O_RDWR);
  if(ffpga<0){
    perror("can't open file /dev/fpgaaccess\n");
    exit(EXIT_FAILURE);
  }

  address = (unsigned char )strtol(argv[1], (char **)NULL, 16);

  if(address%2!=0){
    perror("error address must be pair\n");
    exit(EXIT_FAILURE);
  }
  
  /* write value at address */
  if(argc == 3){
  value   = (unsigned char )strtol(argv[2], (char **)NULL, 16);
  retval = pwrite(ffpga,(void *)&value,2,address);
  if(retval<0){
    perror("write error\n");
    exit(EXIT_FAILURE);
  }
  printf("Wrote %04x at %08x\n",value,address+0x12000000);

  /* read address value */
  }else{
  retval = pread(ffpga,(void *)&value,2,address);
  if(retval<0){
    perror("Read error\n");
    exit(EXIT_FAILURE);
  }
  printf("Read %04x at %08x\n",value,address+0x12000000);
  }

  close(ffpga);
  exit(0);
}
