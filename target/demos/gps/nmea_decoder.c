/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> // O_RDWR
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // sigaction


struct termios oldtio, newtio;

#define DATE_SIZE 6
char date[DATE_SIZE];
struct GPSData 
{
    char time[10+1];
    char latitude[8+1];
    char longitude[9+1];

    char nb_sattelites;
    char altitude[6+1];
};
#define HEADER_SIZE 6
int serialfd;

int openSerialPort( char* aDeviceName, int aSpeed )
{
    int fd;

    fd = open( aDeviceName, O_RDWR | O_NOCTTY );
    if (fd <0) {
        perror(aDeviceName);
        return(-1);
    }

    tcgetattr( fd, &oldtio ); /* save current serial port settings */
    memset( &newtio, sizeof(newtio), 0 ); /* clear struct for new port settings */
// newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD; 
    newtio.c_cflag = aSpeed | CS8 | CLOCAL | CREAD;  /* _no_ CRTSCTS */
    newtio.c_iflag = IGNPAR; //  | ICRNL |IXON; 
    newtio.c_oflag = IGNPAR; // ONOCR|ONLRET|OLCUC;
// newtio.c_lflag = ICANON; 
// newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
// newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
// newtio.c_cc[VERASE]   = 0;     /* del */
// newtio.c_cc[VKILL]    = 0;     /* @ */
// newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 0 character arrives */
// newtio.c_cc[VSWTC]    = 0;     /* '\0' */
// newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
// newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
// newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
// newtio.c_cc[VEOL]     = 0;     /* '\0' */
// newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
// newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
// newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
// newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
// newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    tcflush(fd, TCIFLUSH);
    tcsetattr( fd, TCSANOW, &newtio );

    printf("RS232 Initialization done\n");
    return(fd);
}

void closeSerialPort(int fd)
{
    printf("Restoring old Termio config\n");
    if (fd ) {
        tcsetattr( fd, TCSANOW, &oldtio ); /* restore the old port settings */
        close(fd);
    }
}

int waitForData(int fd)
{
    fd_set fds;
    struct timeval tv;
    int retval;

    FD_ZERO( &fds );
    FD_SET( fd, &fds );
    // Set timeout to 2 secs
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    retval = select( fd+1, &fds, NULL, NULL, &tv );
//    retval = FD_ISSET(fd,&rfds);

    return(retval);
}

unsigned int getParameterFromIndex( unsigned int index, char* aFrame, char* aBuffer, unsigned int buffersize)
{
    char c = 'P';
    int i = 0;

    while( (c != ',') && ( i < buffersize) ) {
        c = aFrame[index++];
        aBuffer[i++] = c;
    }

    aBuffer[i-1] = '\0';
    return(index);
}

struct GPSData gpsdata;

int readAndParseFrame(int fd)
{
    int ret, next_index, count = 0;
    char buf[HEADER_SIZE];
    char frame[128];
    char c;

    // Find start of Frame
    while( c != '$' ) {
        ret = read(fd, &c, 1);
        if (ret < 0) {
            perror("header");
            return(ret);
        }
    }

    // read Header
    while( count < HEADER_SIZE ) {
        ret = read( fd, buf+count, HEADER_SIZE-count);
        if (ret < 0) {
            perror("header");
            return(ret);
        }
        else
            count += ret;
    }

    count = 0;
    while( c != 0x0a ) {
        ret = read(fd, &c, 1);
        if (ret < 0) {
            perror("header");
            return(ret);
        }
        frame[count++] = c;
    }
    ret = read(fd, &c, 1); // read last trailing 0x0a
    printf( "  [%s]", buf );

    if( (ret=strncmp(buf, "GPGGA,", HEADER_SIZE)) == 0) {
        memcpy( date, frame, DATE_SIZE );
        next_index = getParameterFromIndex( 0, frame, gpsdata.time, sizeof(gpsdata.time) );
        next_index = getParameterFromIndex( next_index, frame, gpsdata.latitude, sizeof(gpsdata.latitude) );
    } else {
        ; // don't care
    }
}

void showValues( void )
{
    fprintf(stdout, "\r Time: %s | %s | %s ", date, gpsdata.time, gpsdata.latitude);
    fflush(stdout);
}

void showUsage(void)
{
    printf("Usage: nmea_decoder [ -d DEVICE ] [ -s SPEED ]\n");
    printf(" default device: /dev/ttySMX1\n");
    printf(" default speed: 4800\n");
    exit(1);
}

void signal_handler(int param)
{
    printf("Signal caught %i\n", param);
    closeSerialPort( serialfd );
    exit(0);
}

int main(int argc, char **argv)
{
    struct itimerval t;
    char buf,s[128];
    unsigned char c,flag=0;
    struct sigaction action;

    if (argc <= 1)
        showUsage();

    action.sa_handler = signal_handler;
    sigemptyset( &action.sa_mask );
    sigaddset( &action.sa_mask, SIGTERM );
    action.sa_flags = 0;
    action.sa_restorer = NULL;
    sigaction( SIGINT, &action, /*struct sigaction *oact*/ 0 );

    serialfd = openSerialPort("/dev/ttySMX1", B4800);
    if ( serialfd < 0 )
        exit(1);

    while(1) {
        if( waitForData( serialfd ) ) {
            readAndParseFrame( serialfd );
            showValues();
        } else {
            printf("No data\n");
        }
    }
    closeSerialPort( serialfd );
    exit(0);
}
