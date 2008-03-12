/*
 * fpgaregs.h - Tool to display and modify fpga registers from Linux's userspace
 *
 * Author/Maintainer: --- armadeus -----
 * Derivated from pxaregs (c) Copyright 2002 by M&N Logistik-L�ungen Online GmbH
 * 
 * This tool is placed under the GPL.
 *
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


//#define DEBUG TRUE

// fd for /dev/mem
static int fd = -1;
typedef unsigned short u16;
typedef unsigned int u32;

#define FPGA_BASE_ADDR 0x12000000
// Size of mmapping:
#define MAP_SIZE 4096
#define MAP_MASK ( MAP_SIZE - 1 )


// Get value of the register at the given address
static u16 getmem(u32 addr)
{
   void *map, *regaddr;
   u32 val;
#ifdef DEBUG
   printf("getmem(0x%04x)\n", addr);
#endif // DEBUG
   if (fd == -1) 
   {
      fd = open("/dev/mem", O_RDWR | O_SYNC);
      if (fd<0) 
      {
          perror("open(\"/dev/mem\")");
          exit(1);
      }
   }

   map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK );
   if (map == (void*)-1 ) 
   {
       perror("mmap()");
       exit(1);
   }
   regaddr = map + (addr & MAP_MASK);

   val = *(u32*) regaddr;
   munmap(0,MAP_SIZE);
   printf("read 0x%04x at 0x%08x\n", val, addr);

   return val;
}

// Modify register value at given address
static void putmem(u32 addr, u32 val)
{
   void *map=0;
   void *regaddr=0;
   static int fd = -1;

   //printf("putmem(0x%08x, 0x%08x)\n", addr, val);

   if (fd == -1) 
   {
      fd = open("/dev/mem", O_RDWR | O_SYNC);
      if (fd<0) 
      {
          perror("open(\"/dev/mem\")");
          exit(1);
      }
   }

   map = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr & ~MAP_MASK );
   if (map == (void*)-1 ) 
   {
       perror("mmap()");
       exit(1);
   }

   regaddr = map + (addr & MAP_MASK);
  // printf("Mapped addr: 0x%04x\n", regaddr);

   *(u32*) regaddr = val;
   munmap(0,MAP_SIZE);
}

u16 toInt(char * string)
{   
    char temp;
    u16 result = 0;

    while( *string )
    {
        temp = *string;
        if( (temp>='0') && (temp<='9') )
            temp = temp - '0';
        else if( (temp>='a') && (temp<='f') )
            temp = temp -'a' + 10;

        result = (result<<4) + temp;
        string++;
    } 
    return result;
}


/////////////////////////////////////////////////////////////////////////////////////
// MAIN
//
int main(int argc, char *argv[])
{
    // Dump the content of the provided register
    if (argc == 2) 
    {
       getmem(toInt(argv[1])+FPGA_BASE_ADDR);
       return 0;
    } 

    // Put value to given register
    if (argc == 3) 
    {
        putmem( toInt(argv[1])+FPGA_BASE_ADDR, toInt(argv[2]) );
        return 0;
    }

    // In all other cases, print Usage
    printf("Usage: fpgareg addr [value]\n"
            "addr: address in fpga (max 2ffe)\n"
            "value: value to write (16bit max ffff). Option \n");
           
    return 1;
}

