/*
**	THE ARMADEUS PROJECT
**
**  Copyright (C) year  The source forge armadeus project team
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
**	setDAC: program DAC MAX5821L output
**
**	author: carbure@users.sourceforge.net
*/ 


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// #include <linux/i2c.h> // seems to make trouble during compiling ....
#include <linux/i2c-dev.h>
 
#include <sys/ioctl.h>
#include "max5821.h"
// #define DEBUG_MAX5821		1

/****************************************************************************************************************************/

void usage()
{
	printf ("Usage: setDAC OUTPUT [VALUE]\n");
	printf ("power down or set DAC output with value 0 - 1023 \n\n");
	printf ("  OUTPUT is A , B or AB (both)\n");
	printf ("  VALUE must be 0 - 1023 \n\n");
	printf ("  If no VALUE is given, then the output is powered down \n");
	exit(1);	
}

/****************************************************************************************************************************/

int main(int argc, char *argv[])
{
	int bus;
	int value;
	extended_command	powerDownCommand;
	extended_command	powerUpCommand;
	data_command		setDACOutput;
	unsigned char buf[2];

	if ( (argc !=2) && (argc != 3) )
	{
		usage();
	}

	if ((bus = open("/dev/i2c-0",O_RDWR)) < 0)
	{
		perror("Open error: ");
		exit (1);
	}
	if ( ioctl(bus ,/*I2C_SLAVE*/ 0x703, MAX5821L_IC_SLAVE_ADDRESS_GND) < 0)
	{
		perror("Ioctl error: ");
		exit (1);
	}
	
	powerDownCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
	powerDownCommand.powerMode = POWER_DOWN_MODE0 ;
	powerUpCommand.command = MAX5821_EXTENDED_COMMAND_MODE ;
	powerUpCommand.powerMode = POWER_UP ;

	setDACOutput.Sbits = 0;
	if (0==strcmp(argv[1],"A"))
	{
		powerDownCommand.ctrlA = POWER_CTRL_SELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_UNSELECTED;
		powerUpCommand.ctrlA = POWER_CTRL_SELECTED;
		powerUpCommand.ctrlB = POWER_CTRL_UNSELECTED;
		setDACOutput.command  =  MAX5821_LOAD_DAC_A_IN_REG_B ;
	}
	else if (0==strcmp(argv[1],"B"))
	{
		powerDownCommand.ctrlA = POWER_CTRL_UNSELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_SELECTED;
		powerUpCommand.ctrlA = POWER_CTRL_UNSELECTED;
		powerUpCommand.ctrlB = POWER_CTRL_SELECTED;
	    	setDACOutput.command  =  MAX5821_LOAD_DAC_B_IN_REG_A ;
	}
	else if (0==strcmp(argv[1],"AB"))
	{
		powerDownCommand.ctrlA = POWER_CTRL_SELECTED;
		powerDownCommand.ctrlB = POWER_CTRL_SELECTED;
		powerUpCommand.ctrlA = POWER_CTRL_SELECTED;
		powerUpCommand.ctrlB = POWER_CTRL_SELECTED;
		setDACOutput.command  =  MAX5821_LOAD_DAC_ALL_IN_UPDATE_ALL ;
	}
	else
	{
		usage();
	}

	if (argc == 2 )
	{
		Build_extended_command( powerDownCommand, buf);
#ifdef	DEBUG_MAX5821
		printf("Set the DAC : %02X %02X \n", buf[0],buf[1] );
#endif
	}
	else
	{
		value = atoi(argv[2]);
		if (value >=  MAX5821M_MAX_DATA_VALUE) usage();
		// first, we  power up the DAC j
		Build_extended_command( powerUpCommand, buf);
#ifdef	DEBUG_MAX5821
		printf("Set the DAC : %02X %02X \n", buf[0],buf[1] );
#endif
		if ( 0 !=  write (bus, buf,2))
		{
			printf("Write error\n");
			exit (1);
		}
		setDACOutput.data = value;
		Build_data_command(setDACOutput,buf);
#ifdef	DEBUG_MAX5821
		printf("Set the DAC : %02X %02X \n", buf[0],buf[1] );				
#endif
	}

	if ( 0 !=  write (bus, buf,2))
	{
		printf("Write error\n");
		exit (1);
	}
	exit (0);
}
