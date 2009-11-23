/*
**	THE ARMADEUS PROJECT
**
**  Copyright (C) 2007  The source forge armadeus project team
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
**	ch7024: manage the Svideo controller CH7024
**
**	author: thom25@users.sourceforge.net
*/ 

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <linux/i2c.h> 
#include <linux/i2c-dev.h>
 
#include <sys/ioctl.h>
#include "ch7024.h"
//#define DEBUG_CH7024		1

#define VERSION "0.2"
#define CONF_FILE "ch7024.conf"
#define MAX_DUMP_LINE 100


/* MENU */
void usage()
{
	printf("\nCH7024 utility ver %s. Armadeus\n", VERSION);
	printf("q: quit\n");
	printf("m: modify\n");
	printf("d: dump all\n");
	printf("s: save\n");
	printf("l: load\n");
	printf("other: menu\n");
}

/* Read a byte on the I2C bus
   This is done by writing the register address
   we want to access and then by reading this register
     @param fd: file descriptor of the device
     @param reg: register to access
     @param buf: buffer used to store the result
     @return : -1 in case of error otherwise 0
 */
int read_byte(int fd, unsigned char reg, unsigned char *buf)
{
	/* create an I2C write message (only one byte: the address) */
	struct i2c_msg msg = { CH7024_I2C_SLAVE_ADDR, 0, 1, buf };
	/* create a I2C IOCTL request */
	struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

	buf[0] = reg; /* select reg to read */

	/* write the desired register address */
	if (ioctl(fd, I2C_RDWR, &rdwr) < 0) {
		printf("Write error\n");
		return -1;
	}
	msg.flags = I2C_M_RD; /* read */

	/* read the result and write it in buf[0] */
	if (ioctl(fd, I2C_RDWR, &rdwr) < 0) {
		printf("Read error\n");
		return -1;
	}

	return 0;
}

/* Write a byte on the I2C bus
     @param fd: file descriptor of the device
     @param reg: register to access
     @param value: value to write
     @return : -1 in case of error otherwise 0   
 */
int write_byte(int fd, unsigned char reg, unsigned char value)
{
    unsigned char buf[2] = {reg, value}; /* initialise a data buffer with
                                         address and data */

    /* create an I2C write message */
    struct i2c_msg msg = { CH7024_I2C_SLAVE_ADDR, 0, sizeof(buf), buf };
    /* create a I2C IOCTL request */
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

#ifdef DEBUG_CH7024
    printf("write ref %02X value %02X\n",reg, value);
#endif
    if (ioctl(fd, I2C_RDWR, &rdwr) < 0) {
        printf("Write error\n");
        return -1;
    }

    return 0;
}

/* dump the register values on the screen if destFile is NULL or in a file
     @param string: string to dump
     @param destFile: file in which to write
 */
void dump2Output(char* string, FILE* destFile)
{
    if (destFile == NULL)
        printf("%s", string);
    else
        fprintf(destFile, "%s", string);
}

/* dump the register values on the screen if destFile is NULL or in a file
     @param fd:  file descriptor of the CH7024
     @param destFile: file in which to write if used otherwise ""
 */
void dump(int fd, FILE* destFile)
{
    int i,j, address;
    unsigned char buf[2];
    char dumpString[MAX_DUMP_LINE];
    
    if (destFile == NULL)
        printf("Ad:Va\tRe:Va\tRe:Va\tRe:Va\n");
    for (i=0; i<14; i++) {
        for (j=0; j<4; j++) {
            address = i*4+j;
            if (!read_byte(fd, address, buf)) {
                sprintf(dumpString, "%02X:%02X\t", address, buf[0]);
                dump2Output(dumpString, destFile);
            }
        }
        dump2Output("\n", destFile);
    }
    if (!read_byte(fd, DAC_TRIMMING_CMD, buf)) {
        sprintf(dumpString, "%02X:%02X\t", DAC_TRIMMING_CMD, buf[0]);
        dump2Output(dumpString, destFile);
    }
    if (!read_byte(fd, DATA_IO_CMD, buf)) {
        sprintf(dumpString, "%02X:%02X\t", DATA_IO_CMD, buf[0]);
        dump2Output(dumpString, destFile);
    }
    if (!read_byte(fd, ATTACHED_DISPLAY, buf)) {
        sprintf(dumpString, "%02X:%02X\n", ATTACHED_DISPLAY, buf[0]);
        dump2Output(dumpString, destFile);
    }
}

/* load the CH7024 registers with the values contained in the given file
     @param fd: file descriptor of the CH7024
     @param fileName: file containing the values. If "" then the name has to be entered
 */
void load(int fd, char* fileName)
{
    char line[MAX_DUMP_LINE];
    char tempFileName[MAX_DUMP_LINE];
    FILE *fd_conf = NULL;
    int addr1, addr2, addr3, addr4;
    int val1, val2, val3, val4;
    int nb;

    if (strlen(fileName) == 0) {
        printf("Enter file name: ");
        fgets(tempFileName, sizeof(tempFileName), stdin);
        tempFileName[strlen(tempFileName)-1] = '\0'; /* suppress \n */
    } else {
        strcpy(tempFileName, fileName);    
    }
 
    if ((fd_conf = fopen(tempFileName, "r")) == NULL) {
        perror("Open error: ");
        exit (1);
    }
    while(fgets(line, MAX_DUMP_LINE, fd_conf) != NULL) {
        nb = sscanf(line, "%2X%*c%2X%2X%*c%2X%2X%*c%2X%2X%*c%2X",
                    &addr1, &val1, &addr2, &val2, &addr3, &val3, &addr4, &val4);
        if (nb >= 2)
            write_byte(fd, addr1, val1);
        if (nb >= 4)
            write_byte(fd, addr2, val2);
        if (nb >= 6)
            write_byte(fd, addr3, val3);
        if (nb >= 8)
            write_byte(fd, addr4, val4);
    }
    /* power down to reset the DAC after parameter changing */
    write_byte(fd, POWER_STATE_CMD, 0xFF);
    /* power on the DAC after parameter changing */
    write_byte(fd, POWER_STATE_CMD, 0x00);
    if (fd_conf != NULL)
        fclose(fd_conf);
}


int main(int argc, char *argv[])
{
    int fd; /* CH7024 file descriptor */
    FILE *fd_conf = NULL; /* config file */
    char string[10]; /* user input string */
    unsigned char buf[2]; /* buf to store I2C command/data */
    int regAddr;
    int regValue;

    if (argc > 2) { /* too many args */
        usage();
        exit(1);
    }

    /* open I2C /dev */
    if ((fd = open("/dev/i2c-0", O_RDWR)) < 0) {
        perror("Open error: ");
        exit (1);
    }
    
    /* configure I2C_SLAVE */
    if (ioctl(fd ,/*I2C_SLAVE*/ 0x703, CH7024_I2C_SLAVE_ADDR) < 0) {
        perror("Ioctl error: ");
        exit (1);
    }

    /* check CH7024 presence */
    if (!read_byte( fd, DEVICE_ID_CMD, buf)) {
        if (buf[0] != CH7024_ID) {
            printf("CH7024 not recognized. Exit\n");
            exit(1);
        }
    } else {
        printf("CH7024 not found. Exit\n");
        exit(1);
    }
        
    if (argc == 2) { /* read data from file */
        load(fd,argv[1]);
        exit(0);
    }

    usage();
    while(1) {
        fgets(string, sizeof(string), stdin);
        if (string[0] == 'q')  /* Exit */
            exit(0);
        else if (string[0] == 'm') { /* Modify registers */
            printf("reg addr (hex without 0x): ");
            fgets(string, sizeof(string), stdin);
            sscanf(string, "%2x", &regAddr);
            printf("reg value (hex without 0x): ");
            fgets(string, sizeof(string), stdin);
            sscanf(string,"%2x", &regValue);
            write_byte(fd, regAddr, regValue);
        }
        else if (string[0] == 'd')   /* Dump to screen */
            dump(fd,fd_conf);
        else if (string[0] == 's') {  /* Save to file */
            if ((fd_conf = fopen(CONF_FILE, "w")) < 0) {
                perror("Open error: ");
                exit (1);
            }
            dump(fd, fd_conf);
            if (fd_conf != NULL)
                fclose(fd_conf);
        }
        else if (string[0] == 'l') { /* load from file */
            load(fd,"");
        }
        else
            usage();
    };
    
    if (fd >= 0)
        close(fd);
  
    exit (0);
}
