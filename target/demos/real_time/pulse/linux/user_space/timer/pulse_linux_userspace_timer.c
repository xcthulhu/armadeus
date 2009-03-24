#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TIMESLEEP 1
#define PORT "/dev/gpio/PA4"

int fd;
int iomask;
void test() {
	printf("hello world\n");
	write(fd, &iomask, sizeof(iomask));
	iomask ^=1;
}

int main(int argc, char **argv) {
	printf("**** Générateur d'impulsions avec timer en mode utilisateur ****\n");
 	
	fd = open(PORT,O_RDWR);
	if (fd < 0) {
		printf("erreur\n");
		return 1;
	}
	iomask = 1;
	do {
		signal(SIGALRM,test);
		alarm(TIMESLEEP);
		pause();
	} while(1);
    return 0;
}
