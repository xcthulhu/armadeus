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
