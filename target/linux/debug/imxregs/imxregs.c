/*
 * imxregs.c - Tool to display and modify iMX registers from Linux's userspace
 *
 * Maintainer: J. Boibessot
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

/*#include <linux/i2c.h>
#include <linux/i2c-dev.h>*/

#include "imxregs.h"

//#define DEBUG TRUE

// fd for /dev/mem
static int fd = -1;


// Get value of the register at the given address
static int getmem(u32 addr)
{
   void *map, *regaddr;
   u32 val;
#ifdef DEBUG
   printf("getmem(0x%08x)\n", addr);
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
#ifdef DEBUG
   printf("Mapped addr: 0x%x\n", regaddr);
#endif // DEBUG
   regaddr = map + (addr & MAP_MASK);

   val = *(u32*) regaddr;
   munmap(0,MAP_SIZE);

   return val;
}

// Modify register value at given address
static void putmem(u32 addr, u32 val)
{
   void *map, *regaddr;
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
   printf("Mapped addr: 0x%x\n", regaddr);
   regaddr = map + (addr & MAP_MASK);

   *(u32*) regaddr = val;
   munmap(0,MAP_SIZE);
}

static u32 lastaddr = 0;
static u32 newaddr = 1;
static u32 data = 0;
static u32 shiftdata;

//
// Show content of given register (index in reginfo struct)
static void dumpentry(int i)
{
   int j;

   if (regs[i].addr != lastaddr) newaddr = 1;
   if (newaddr) 
   {
       newaddr = 0;
       lastaddr = regs[i].addr;
       data = getmem(lastaddr);
       printf("\n%s\n", regs[i].desc);
       printf("%-24s 0x%08x  -> ", regs[i].name, data);
       shiftdata = data;
       for (j=32; j>0; j--) {
           printf("%c", shiftdata & 0x80000000 ? '1' : '0');
           shiftdata = shiftdata << 1;
           if (j==9 || j==17 || j==25) printf(" ");
       }

       printf("\n");
   }

   if( (regs[i].shift != 0)  || (regs[i].mask != 0xffffffff) ) 
   {
       shiftdata = (data >> regs[i].shift) & regs[i].mask;
       printf("%-25s  ", regs[i].name);
       switch (regs[i].type) 
       {
          // Hexa printing
          case 'x': 
            printf("%8x", shiftdata);
          break;
          case '<': 
            printf("%8u", 1 << shiftdata);
          break;
          // Decimal printing
          default:
            printf("%8u", shiftdata);
          break;
       }
       printf("  %s\n", regs[i].desc);
   }
}

//
// Show content of all supported registers
static void dumpall(void)
{
   int i;
   int n=sizeof(regs)/sizeof(struct reg_info);

   for( i=0; i<n; i++ ) 
   {
      dumpentry(i);
   }
}

//
// Show content of register with name starting with given string
static void dumpmatching(char *name)
{
   int i, found=0;
   int n=sizeof(regs)/sizeof(struct reg_info);

   for (i=0; i<n; i++) 
   {
      if (strstr(regs[i].name, name)) {
         dumpentry(i);
         found = 1;
      }
   }
   if(!found)
      printf("No matching register found\n");
}

//
// Set content of register with given name
static void setreg(char *name, u32 val)
{
   int i;
   u32 mem;
   int found=0;
   int count=0;
   int n=sizeof(regs)/sizeof(struct reg_info);

   for (i=0; i<n; i++) 
   {
      if (strcmp(regs[i].name, name)==0) 
      {
         found = i;
         //printf("Matched %s with %s, count=%d\n", regs[i].name, name, count);
         count++;
      }
   }
   if (count!=1) 
   {
      printf("No or more than one matching register found\n");
      exit(1);
   }

   mem = getmem(regs[found].addr);
   //printf("Old contents: 0x%08x\n", mem);
   mem &= ~(regs[found].mask << regs[found].shift);
   //printf("Unmasked contents: 0x%08x\n", mem);
   val &= regs[found].mask;
   //printf("mask: 0x%08x\n", regs[found].mask);
   //printf("masked val: 0x%08x\n", val);
   mem |= val << regs[found].shift;
   //printf("Embedded value: 0x%08x\n", mem);
   putmem(regs[found].addr, mem);   
}

/////////////////////////////////////////////////////////////////////////////////////
// MAIN
//
int main(int argc, char *argv[])
{
    char *p;
    u32 val;

    // No arguments -> dump all known registers
    if (argc == 1) {
       dumpall();
       return 0;
    }

    // Uppercase first argument (register name)
    if (argc >= 2) 
    {
        p = argv[1];
        while (*p) {
           *p = toupper(*p);
           p++;
        }
    }

    // Dump the content of the provided register
    if (argc == 2) 
    {
       dumpmatching(argv[1]);
       return 0;
    }

    // Put value to given register
    if (argc == 3) 
    {
        sscanf(argv[2],"%i",&val);
        setreg(argv[1], val);
        return 0;
    }

    // In all other cases, print Usage
    printf("Usage: %s                - to dump all known registers\n"
           "       %s <name>         - to dump named register\n"
           "       %s <name> <value> - to set named register\n",
           argv[0], argv[0], argv[0]);
           
    return 1;
}

