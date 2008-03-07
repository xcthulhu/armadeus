/* a little program to test led driver
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * 7 march 2008
 */

#include <stdio.h>
#include <stdlib.h>

/* file management */
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>

/* sleep */
#include <unistd.h>

int fled;

void quit(int pouet){
  close(fled);
  exit(0);
}

int main(int argc, char *argv[])
{
  unsigned short i,j;

  /* quit when Ctrl-C pressed */
  signal(SIGINT, quit);

  j=0;

  printf( "Testing led driver\n" );

  if(argc < 2){
    perror("invalide arguments number\ntestled <led_filename>\n");
    exit(EXIT_FAILURE);
  }

  fled=open(argv[1],O_RDWR);
  if(fled<0){
    perror("can't open file\n");
    exit(EXIT_FAILURE);
  }

  while(1){
    i = (i==0)?1:0;
    printf("Write %d,",i);
    fflush(stdout);
    /* write i in led register */
    if(write(fled, &i, 1)<0){
      perror("write error\n");
      exit(EXIT_FAILURE);
    }
    if(read(fled,&j,1)<0){
      perror("read error\n");
      exit(EXIT_FAILURE);
    }
    printf("Read %d\n",j);

    if(lseek(fled,0,SEEK_SET)<0){
      perror("lseek error\n");
      exit(EXIT_FAILURE);
    }

    sleep(1);/* wait one second */
  }

  close(fled);
  exit(0);
}
