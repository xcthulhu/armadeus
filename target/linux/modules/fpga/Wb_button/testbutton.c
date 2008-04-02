/* a little program to test button driver
 * Fabien Marteau <fabien.marteau@armadeus.com>
 * 7 march 2008
 */

#include <stdio.h>
#include <stdlib.h>

/* file management */
#include <sys/stat.h>
#include <fcntl.h>

/* as name said */
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

  printf( "Testing button driver\n" );

  if(argc < 2){
    perror("invalide arguments number\ntestled <button_filename>\n");
    exit(EXIT_FAILURE);
  }

  fled=open(argv[1],O_RDWR);
  if(fled<0){
    perror("can't open file\n");
    exit(EXIT_FAILURE);
  }

  while(1){
    i = (i==0)?1:0;
    fflush(stdout);

    /* read value */
    if(read(fled,&j,1)<0){
      perror("read error\n");
      exit(EXIT_FAILURE);
    }
    printf("Read %d\n",j);

    if(lseek(fled,0,SEEK_SET)<0){
      perror("lseek error\n");
      exit(EXIT_FAILURE);
    }
  }

  close(fled);
  exit(0);
}
