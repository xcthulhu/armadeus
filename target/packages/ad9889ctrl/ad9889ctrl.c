/*
**	THE ARMADEUS PROJECT
**
**  Copyright (C) 2009  The source forge armadeus project team
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
**	ad9889ctrl: manage the AD9889 Video controller
**
**	authors: thom25@users.sourceforge.net, jorasse@armadeus.org
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
#include "ad9889ctrl.h"
#define DEBUG_AD9889		1

#define VERSION "0.2alpha"
#define CONF_FILE "ad9889.conf"
#define MAX_DUMP_LINE 100
/****************************************************************************************************************************/

/* MENU */
void usage()
{
	printf("\nad9889ctrl utility ver %s Armadeus\n", VERSION);
	printf("q: quit\n");
	printf("i: initialize\n");
	printf("m: modify\n");
	printf("d: dump AD9889 registers\n");
	printf("e: dump EDID registers\n");
	printf("s: save\n");
	printf("l: load\n");
	printf("other: this menu\n");
}

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, 
                                     int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file, I2C_SMBUS, &args);
}

/* Read a byte on the I2C bus
   This is done by writing the register address
   we want to access and then by reading this register
     @param fd: file descriptor of the device
     @param dev: address of the device on I2C bus
     @param reg: register to access
     @param buf: buffer used to store the result
     @return : -1 in case of error otherwise 0
 */
int read_byte(int fd, unsigned char dev, unsigned char reg, unsigned char *buf)
{
	union i2c_smbus_data data;

	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, dev) < 0) {
		perror("Ioctl error: ");
		return -1;
	}

	if (i2c_smbus_access(fd, I2C_SMBUS_READ, reg,
	                     I2C_SMBUS_BYTE_DATA, &data)) {
		printf("Read error\n");
		return -1;
	} else {
		buf[0] = 0x0FF & data.byte;
		return 0;
	}
}

/* Write a byte on the I2C bus
     @param fd: file descriptor of the device
     @param dev: address of the device on I2C bus
     @param reg: register to access
     @param value: value to write
     @return : -1 in case of error otherwise 0   
 */
int write_byte(int fd, unsigned char dev, unsigned char reg, unsigned char value)
{
	union i2c_smbus_data data;
	data.byte = value;

#ifdef DEBUG_AD9889
	printf("write chip %02X ref %02X value %02X\n", dev, reg, value);
#endif
	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, dev) < 0) {
		perror("Ioctl error: ");
		return -1;
	}

	return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg,
	                        I2C_SMBUS_BYTE_DATA, &data);

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
     @param fd:  file descriptor of the AD9889
     @param destFile: file in which to write if used otherwise ""
 */
void dump(int fd, FILE* destFile)
{
	int i,j, address;
	unsigned char buf[2];
	char dumpString[MAX_DUMP_LINE];
	unsigned char dev = AD9889_I2C_SLAVE_ADDR;
    
	if (destFile == NULL)
		printf("Dev:Ad:Va\tDev:Ad:Va\tDev:Ad:Va\tDev:Ad:Va\n");

	/* dumping main registers */
	for (i = 0; i<(0xd0/4); i++) {
		for (j = 0; j<4; j++) {
			address = i*4+j;
			if (!read_byte(fd, dev, address, buf)) {
				sprintf(dumpString, " %02X:%02X:%02X\t",
					AD9889_I2C_SLAVE_ADDR, address, buf[0] );
				dump2Output(dumpString, destFile);
			}
		}
		dump2Output("\n", destFile);
	}

	/* dumping secondary registers */
	if (!read_byte(fd, AD9889_I2C_SLAVE_ADDR, 0xCF, buf)) {
		dev = buf[0]>>1;
		for (address = 0; address<0x1F; address++) {
			if (!read_byte( fd, dev, address, buf)) {
				sprintf(dumpString, " %02X:%02X:%02X\t",
					dev, address, buf[0]);
				dump2Output(dumpString, destFile);
			}
			if ((3 == address % 4) && (0x1E != address)) {
				dump2Output("\n", destFile);
			}
		}
		dump2Output("\n", destFile);
	}
/*
    if( !read_byte( fd, DAC_TRIMMING_CMD, buf ) ){
        sprintf( dumpString, "%02X:%02X\t", DAC_TRIMMING_CMD, buf[0] );
        dump2Output( dumpString, destFile );
    }
    if( !read_byte( fd, DATA_IO_CMD, buf ) ){
        sprintf( dumpString, "%02X:%02X\t", DATA_IO_CMD, buf[0] );
        dump2Output( dumpString, destFile );
    }
    if( !read_byte( fd, ATTACHED_DISPLAY, buf ) ){
        sprintf( dumpString, "%02X:%02X\n", ATTACHED_DISPLAY, buf[0] );
        dump2Output( dumpString, destFile );
    }
*/
}

/* dump the register values on the screen if destFile is NULL or in a file
     @param fd:  file descriptor of the AD9889
     @param destFile: file in which to write if used otherwise ""
 */
void dumpedid(int fd, FILE* destFile)
{
	int address;
	unsigned char buf[2];
	char dumpString[MAX_DUMP_LINE];
	unsigned char dev = AD9889_I2C_SLAVE_ADDR;
    
	if (destFile == NULL)
		printf("Dev:Ad:Va\tDev:Ad:Va\tDev:Ad:Va\tDev:Ad:Va\n");

	/* dumping EDID memory */
	if (!read_byte(fd, AD9889_I2C_SLAVE_ADDR, 0x43, buf)) {
		dev = buf[0]>>1;
		for (address = 0; address<0x100; address++) {
			if (!read_byte(fd, dev, address, buf)) {
				sprintf(dumpString, " %02X:%02X:%02X\t",
					dev, address, buf[0] );
				dump2Output(dumpString, destFile);
			}
			if ((3 == address % 4) && (0xFF != address)) {
				dump2Output("\n", destFile);
			}
		}
		dump2Output("\n", destFile);
	}
/*
    if( !read_byte( fd, DAC_TRIMMING_CMD, buf ) ){
        sprintf( dumpString, "%02X:%02X\t", DAC_TRIMMING_CMD, buf[0] );
        dump2Output( dumpString, destFile );
    }
    if( !read_byte( fd, DATA_IO_CMD, buf ) ){
        sprintf( dumpString, "%02X:%02X\t", DATA_IO_CMD, buf[0] );
        dump2Output( dumpString, destFile );
    }
    if( !read_byte( fd, ATTACHED_DISPLAY, buf ) ){
        sprintf( dumpString, "%02X:%02X\n", ATTACHED_DISPLAY, buf[0] );
        dump2Output( dumpString, destFile );
    }
*/
}

/* load the AD9889 registers with the values contained in the given file
     @param fd: file descriptor of the AD9889
     @param fileName: file containing the values. If "" then the name has to be entered
 */
void load(int fd, char* fileName)
{
    char line[MAX_DUMP_LINE];
    char tempFileName[MAX_DUMP_LINE];
    FILE *fd_conf = NULL;
    int dev1, dev2, dev3, dev4;
    int addr1, addr2, addr3, addr4;
    int val1, val2, val3, val4;
    int nb;

    if (strlen(fileName) == 0) {
        printf("Enter file name: ");
        fgets(tempFileName, sizeof(tempFileName), stdin);
        tempFileName[strlen(tempFileName)-1] = '\0'; /* suppress \n */
    }
    else {
        strcpy(tempFileName, fileName);
    }
 
    if ((fd_conf = fopen(tempFileName, "r")) == NULL) {
        perror("Open error: ");
        exit (1);
    }
    while (fgets(line, MAX_DUMP_LINE, fd_conf) != NULL) {
        nb = sscanf(line,"%2X%*c%2X%*c%2X%2X%*c%2X%*c%2X%2X%*c%2X%*c%2X%2X%*c%2X%*c%2X",
			&dev1, &addr1, &val1, &dev2, &addr2, &val2,
			&dev3, &addr3, &val3, &dev4, &addr4, &val4 );
        if (nb >= 3)
            write_byte (fd, dev1, addr1, val1);
        if (nb >= 6)
            write_byte (fd, dev2, addr2, val2);
        if (nb >= 9)
            write_byte (fd, dev3, addr3, val3);
        if (nb >= 12)
            write_byte (fd, dev4, addr4, val4);
    }
    /* power down to reset the dac after parameter changing*/
//    write_byte (fd, POWER_STATE_CMD, 0xFF);
    /* power on the dac after parameter changing*/
//    write_byte (fd, POWER_STATE_CMD, 0x00);
    if (fd_conf != NULL)
        fclose(fd_conf);
}

int ad9889_init(int fd)
{
	unsigned char buf[2];
	int hpd = 0;
	int i= 10000;

	while ((!hpd) && (i-- >0)) {
		if (!read_byte(fd, AD9889_I2C_SLAVE_ADDR, 0x42, buf)) {
			hpd = buf[0]&0x40;
		} else {
                    printf("hpd read failed\n");
                }
	}

	if (hpd) {
		printf("hpd active.\n");
	} else {
		printf("hpd not active, force powerup.\n");
	}
	write_byte(fd, AD9889_I2C_SLAVE_ADDR, 0x41, 0x10);

	return 0;
}

int main(int argc, char *argv[])
{
	int fd; 		// AD9889 file descriptor
	FILE *fd_conf = NULL; 	// config file
	char string[10];	// user input string
	unsigned char buf[2];	// buf to store I2C command/data
	int regAddr;
	int regValue;
	char* bus = "/dev/i2c-1";

	if (argc > 2) { /* too many args */
		usage();
		exit(1);
	} else if (argc == 2) { /* i2c bus */
		bus = argv[1];
	}

	/* open I2C /dev */
	if ((fd = open(bus, O_RDWR)) < 0) {
		perror("Open error: ");
		exit (1);
	}
    
	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, AD9889_I2C_SLAVE_ADDR) < 0) {
		perror("Ioctl error: ");
		exit (1);
	}

	/* check AD9889 presence */
	if (!read_byte(fd, AD9889_I2C_SLAVE_ADDR, CHIP_REVISION, buf)) {
		if ((AD9889_MAX_REV < buf[0])) {
			printf("AD9889 revision not supported. Exiting\n");
			exit(1);
		}
	} else {
		printf("AD9889 not found. Exiting\n");
		exit(1);
	}
        
	usage();
	while(1) {
		fgets(string, sizeof(string), stdin);
		if (string[0] == 'q') {		/* Exit */
			exit(0);
		}
		else if (string[0] == 'm') {	/* Modify registers */
			int dev;
			printf("chip addr (hex without 0x)(ex 39): ");
			fgets(string, sizeof(string), stdin);
			sscanf(string, "%2x", &dev);
			printf("reg addr (hex without 0x): ");
			fgets(string, sizeof(string), stdin);
			sscanf(string, "%2x", &regAddr);
			printf("reg value (hex without 0x): ");
			fgets(string, sizeof(string), stdin);
			sscanf(string, "%2x", &regValue);
			write_byte(fd, dev, regAddr, regValue);
		}
		else if (string[0] == 'd') {	/* Dump to screen */
			dump(fd, fd_conf);
		}
		else if (string[0] == 'e') {	/* Dump to screen */
			dumpedid(fd, NULL);
		}
		else if (string[0] == 's') {	/* Save to file */
			if ((fd_conf = fopen(CONF_FILE, "w")) < 0) {
				perror("Open error: ");
				exit (1);
			}
			dump(fd, fd_conf);
			if (fd_conf != NULL) {
				fclose(fd_conf);
			}
		}
		else if (string[0] == 'l') {	/* load from file */
			load(fd, "");
		}
		else if (string[0] == 'i') {	/* init ad9889 */
			printf("initialize...\n");
			ad9889_init(fd);
		} else {
			usage();
		}
	};
    
	if (fd >= 0) {
		close(fd);
	}
  
	exit (0);
}

