/************************************************************************
 * a program to write/read 16/32bits values on fpga address map
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


#define WORD_ACCESS (2)
#define LONG_ACCESS (4)

void displayResult(char* text, unsigned int accesstype, unsigned int value, unsigned int address)
{
  if( accesstype == WORD_ACCESS )
    printf("%s %04x at %08x\n",text, (unsigned short)value,address);
  else
    printf("%s %08x at %08x\n",text, value,address);
}


int main(int argc, char *argv[])
{
  unsigned short address;
  unsigned int value ;
  int retval, error = EXIT_SUCCESS;
  int ffpga, accesstype = LONG_ACCESS;
  
  if((argc < 3) || (argc > 4)){
    printf("invalid arguments number\nfpgaregs [w,l] fpga_reg_add [value].\n\tw: word access, l: long access\n\
\tEx: fpgaregs w 0x10 0x1234, fpgaregs l 0x10 0x12345678\n");
    exit(EXIT_FAILURE);
  }

  ffpga=open("/dev/fpgaaccess",O_RDWR);
  if(ffpga<0){
    printf("can't open file /dev/fpgaaccess\n");
    exit(EXIT_FAILURE);
  }

  address = (unsigned int)strtol(argv[2], (char **)NULL, 16);

  if(argc<3){
    printf("invalid command line");
  }
  else {  
    if( *(argv[1]) == 'w' )
      accesstype = WORD_ACCESS;
     
    /* write value at address */
    if(argc == 4){ 
      value   = strtoul(argv[3], (char **)NULL, 16);
      retval = pwrite(ffpga,(void *)&value,accesstype,address);
      if(retval<0){
        printf("write error\n");
        error = EXIT_FAILURE;
      }
      else
        displayResult("Write", accesstype, value, address);
        
      /* read address value */
    }else if(argc == 3){
      retval = pread(ffpga,(void *)&value,accesstype,address);
      if(retval<0){
        perror("Read error\n");
        error = EXIT_FAILURE;
      }
      else
        displayResult("Read", accesstype, value, address);
    }
  }
  close(ffpga);
  exit(error);
}

