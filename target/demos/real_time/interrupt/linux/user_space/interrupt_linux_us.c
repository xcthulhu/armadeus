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
//struct sigaction oa;
// struct sigaction sa;
 // sa.sa_sigaction = action;
  //sa.sa_flags = 0;
  //sa.sa_restorer = NULL;
  //sigemptyset(&sa.sa_mask);

  //sigaction (SIGIO, &sa, &oa);

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
  //retval = 0;
  // enable the gpio async notifications
  //oflags = fcntl (fd, F_GETFL);
  //retval = fcntl (fd, F_SETFL, oflags | FASYNC);
  /*if (retval < 0)
    {
      printf ("F_SETFL Fails \n");
    }*/
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
