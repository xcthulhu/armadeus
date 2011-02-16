/*
**	ARMADEUS Systems
**
**  Copyright (C) 2011  The source forge armadeus systems team
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
**	tfp410ctrl: manage the TFP410 Video controller
**
**	authors: thom25@users.sourceforge.net
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
#include "tfp410ctrl.h"
#define DEBUG_TFP410		1

#define VERSION "0.2alpha"
#define CONF_FILE "tfp410.conf"
#define MAX_DUMP_LINE 100
/****************************************************************************************************************************/

/* MENU */
void usage()
{
	printf("\ntfp410ctrl utility ver %s Armadeus\n", VERSION);
	printf("q: quit\n");
	printf("m: modify\n");
	printf("d: dump tfp410 registers\n");
	printf("s: save\n");
	printf("l: load\n");
	printf("other: this menu\n");
}

void registerMenu()
{
	printf("\n1. CTL_1_MODE");
	printf("\n2. CTL_2_MODE");
	printf("\n3. CTL_3_MODE");
	printf("\n4. CFG");
	printf("\n5. DE_DLY");
	printf("\n6. DE_CTL");
	printf("\n7. DE_TOP");
	printf("\n8. DE_CNT");
	printf("\n9. DE_LIN\n");
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
		*buf = 0x0FF & data.byte;
		return 0;
	}
}

int read_word(int fd, unsigned char dev, unsigned char reg, unsigned short *buf)
{
	union i2c_smbus_data data;

	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, dev) < 0) {
		perror("Ioctl error: ");
		return -1;
	}

	if (i2c_smbus_access(fd, I2C_SMBUS_READ, reg,
	                     I2C_SMBUS_WORD_DATA, &data)) {
		printf("Read error\n");
		return -1;
	} else {
		*buf = 0xFFFF & data.word;
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

	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, dev) < 0) {
		perror("Ioctl error: ");
		return -1;
	}

	return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg,
	                        I2C_SMBUS_BYTE_DATA, &data);

    return 0;
}

int write_word(int fd, unsigned char dev, unsigned char reg, unsigned short value)
{
	union i2c_smbus_data data;
	data.word = value;

	/* configure I2C_SLAVE */
	if (ioctl(fd, I2C_SLAVE, dev) < 0) {
		perror("Ioctl error: ");
		return -1;
	}

	return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg,
	                        I2C_SMBUS_WORD_DATA, &data);

    return 0;
}

/* dump the register values on the screen if destFile is NULL or in a file
 */
void dumpReg(int fd, int regAddr, char* info, FILE* destFile)
{
	unsigned short buf = 0;
	char dumpString[MAX_DUMP_LINE];

	if (regAddr > DE_TOP_REG)
		read_word(fd, TFP410_I2C_SLAVE_ADDR, regAddr, &buf);
	else
		read_byte(fd, TFP410_I2C_SLAVE_ADDR, regAddr, (unsigned char*)&buf);

	if (destFile == NULL){
		sprintf(dumpString, "%s %02X:%04X\t", info, regAddr, buf);
		printf("%s\n", dumpString);
	} else {
		sprintf(dumpString, "%02X:%04X\t", regAddr, buf);
	        fprintf(destFile, "%s\n", dumpString);
	}
}

/* dump the register values on the screen if destFile is NULL or in a file
     @param fd:  file descriptor of the TFP410
     @param destFile: file in which to write if used otherwise ""
 */
void dump(int fd, FILE* destFile)
{
    
	if (destFile == NULL)
		printf("\t\t Ad:Va\n");

	dumpReg(fd, CTL_1_MODE_REG, "CTL_1_MODE:\t", destFile);
	dumpReg(fd, CTL_2_MODE_REG, "CTL_2_MODE:\t", destFile);
	dumpReg(fd, CTL_3_MODE_REG, "CTL_3_MODE:\t", destFile);
	dumpReg(fd, CFG_REG, "CFG:\t\t", destFile);
	dumpReg(fd, DE_DLY_REG, "DE_DLY:\t\t", destFile);
	dumpReg(fd, DE_CTL_REG, "DE_CTL:\t\t", destFile);
	dumpReg(fd, DE_TOP_REG, "DE_TOP:\t\t", destFile);
	dumpReg(fd, DE_CNT_REG, "DE_CNT:\t\t", destFile);
	dumpReg(fd, DE_LIN_REG, "DE_LIN:\t\t", destFile);
	dumpReg(fd, H_RES_REG, "H_RES:\t\t", destFile);
	dumpReg(fd, V_RES_REG, "V_RES:\t\t", destFile);
}

/* load the TFP410 registers with the values contained in the given file
     @param fd: file descriptor of the TFP410
     @param fileName: file containing the values. If "" then the name has to be entered
 */
void load(int fd, char* fileName)
{
    char line[MAX_DUMP_LINE];
    char tempFileName[MAX_DUMP_LINE];
    FILE *fd_conf = NULL;
    unsigned int regAddr, val;

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
        sscanf(line,"%2X%*c%4X", &regAddr, &val);
	printf("reg %02x, value %04x\n", regAddr, val);
	if (regAddr > DE_TOP_REG)
	            write_word (fd, TFP410_I2C_SLAVE_ADDR, regAddr, val);
	else
	            write_byte (fd, TFP410_I2C_SLAVE_ADDR, regAddr, val);
    }
    if (fd_conf != NULL)
        fclose(fd_conf);
}

void modifyRegister(int fd, int regAddr)
{
	int regValue;
	char string[10];	// user input string

	printf("Reg value:");
	fgets(string, sizeof(string), stdin);
	if ((regAddr == DE_CNT_REG) || (regAddr == DE_LIN_REG)) {
		sscanf(string, "%4x", &regValue);
		write_word(fd, TFP410_I2C_SLAVE_ADDR, regAddr, regValue);	
	} else {
		sscanf(string, "%2x", &regValue);
		write_byte(fd, TFP410_I2C_SLAVE_ADDR, regAddr, regValue);
	}
}

void tfp410_init( int fd )
{
	/* exit power down mode */
	write_byte(fd, TFP410_I2C_SLAVE_ADDR, CTL_1_MODE_REG, 0xBD);
	/* configure MSEN/PO1 as hotplug detect pin */ 
	write_byte(fd, TFP410_I2C_SLAVE_ADDR, CTL_2_MODE_REG, 0x32);

}

int main(int argc, char *argv[])
{
	int fd; 		// TFP410 file descriptor
	FILE *fd_conf = NULL; 	// config file
	char string[10];	// user input string
	unsigned short buf;	// buf to store I2C command/data
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
	if (ioctl(fd, I2C_SLAVE, TFP410_I2C_SLAVE_ADDR) < 0) {
		perror("Ioctl error: ");
		exit (1);
	}

	/* check TFP410 presence */
	if (!read_word(fd, TFP410_I2C_SLAVE_ADDR, DEV_ID_REG, &buf)) {
		printf("DEV_ID = %x\n", buf);
		if ((DEV_ID != buf)) {
			printf("incorrect dev id. Exiting\n");
			exit(1);
		}
	} else {
		printf("TFP410 not found. Exiting\n");
		exit(1);
	}
        
	tfp410_init(fd);
	usage();
	while(1) {
		fgets(string, sizeof(string), stdin);
		if (string[0] == 'q') {		/* Exit */
			exit(0);
		}
		else if (string[0] == 'm') {	/* Modify registers */
			registerMenu();
			fgets(string, sizeof(string), stdin);
			switch (string[0]){
				case '1': modifyRegister(fd, CTL_1_MODE_REG); break;
				case '2': modifyRegister(fd, CTL_2_MODE_REG); break;
				case '3': modifyRegister(fd, CTL_3_MODE_REG); break;
				case '4': modifyRegister(fd, CFG_REG); break;
				case '5': modifyRegister(fd, DE_DLY_REG); break;
				case '6': modifyRegister(fd, DE_CTL_REG); break;
				case '7': modifyRegister(fd, DE_TOP_REG); break;
				case '8': modifyRegister(fd, DE_CNT_REG); break;
				case '9': modifyRegister(fd, DE_LIN_REG); break;
				default: printf("invalid selection\n"); break;
			}
		}
		else if (string[0] == 'd') {	/* Dump to screen */
			dump(fd, fd_conf);
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
			printf("Data saved to conf file\n");
		}
		else if (string[0] == 'l') {	/* load from file */
			load(fd, "");
		} else {
			usage();
		}
	};
    
	if (fd >= 0) {
		close(fd);
	}
  
	exit (0);
}

