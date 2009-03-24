#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "lcd.h"

#define RMAX  2.
#define RMIN -3.
#define IMAX  2.5
#define IMIN -2.5

char *fbp = 0;


struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;


int init_lcd(){
  // Open the file for reading and writing
  int fbfd = open("/dev/fb0", O_RDWR);
  if (!fbfd) {
    printf("Error: cannot open framebuffer device.\n");
    return -1;
  }
  printf("The framebuffer device was opened successfully.\n");
  
  // Get fixed screen information
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    printf("Error reading fixed information.\n");
    return -1;
  }
  
  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    printf("Error reading variable information.\n");
    return -1;
  }
  
  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );

  // Figure out the size of the screen in bytes
  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
  
  // Map the device to memory
  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
		     fbfd, 0);
  if ((int)fbp == -1) {
    printf("Error: failed to map framebuffer device to memory.\n");
    return -1;
  }
  printf("The framebuffer device was mapped to memory successfully.\n");
  return fbfd;
  }

void close_LCD(int fbfd){
  munmap(fbp, screensize);
  close(fbfd);
  }


void print_pix(int x, int y, int r, int g, int b){
  long int location = 0;
  if(x>=320)x=319;
  if(y>=240)x=239;
  if(x<0)x=0;
  if(y<0)x=0;
  location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
    (y+vinfo.yoffset) * finfo.line_length;
  
  unsigned short int t = ((r&0xf8)>>3)<<11 | ((g&0xfc)>>2) << 5 | ((b&0xf8)>>3);
  *((unsigned short int*)(fbp + location)) = t; 
}

