/*
**    THE ARMadeus Systems
** 
**    Copyright (C) 2009  The armadeus systems team 
**    Fabien Marteau <fabien.marteau@armadeus.com>
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
** TODO: manage force correctly
*/

#include "as_i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define I2C_DEV_PATH "/dev/i2c-"

/* global static variable */
static int mFHandlerI2c[NUMBER_OF_I2C];

/*------------------------------------------------------------------------------*/

/** Init i2c bus 
 *
 * @param aBusNumber i2c bus number
 * @return error code
 */
int 
as_i2c_init(int aBusNumber)
{
    char buffer[40];
    /* make path */
    snprintf(buffer,39,"%s%d",I2C_DEV_PATH,aBusNumber);
    /* open i2c /dev */
    if((mFHandlerI2c[aBusNumber] = open(buffer,O_RDWR))<0)
    {
        perror("Open error : ");
        return -1;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/

/** read byte on i2c bus
 * @param aBusNumber i2c bus number
 * @param aChipAddr chip address
 * @param aReg Register number
 * @param *aBuf char buffer
 * @return error code
 */
int 
as_read_byte_data(int aBusNumber,
                  unsigned char aChipAddr,
                  unsigned char aReg,
                  unsigned char *aBuf)
{
    // create an I2C write message (only one byte: the address)
    struct i2c_msg msg = { aChipAddr, 0, 1, aBuf };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    aBuf[0] = aReg; // select aReg to read

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c[aBusNumber], I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		return -1;
 	}

    // write the desired aRegister address
	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}
    msg.flags = I2C_M_RD; // read

    // read the result and write it in aBuf[0]
	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Read error\n");
		return -1;
	}
    return 0;

}

/*------------------------------------------------------------------------------*/

/** write byte on i2c bus
 * @param aChipAddr chip address
 * @param aReg Register number
 * @param aBuf char value to write
 * @return error return
 */
int 
as_write_byte_data(int aBusNumber,
                   unsigned char aChipAddr,
                   unsigned char aReg,
                   unsigned char aValue)
{
    unsigned char buf[2] = {aReg,aValue}; // initialise a data buffer with
                                          // address and data

    // create an I2C write message
    struct i2c_msg msg = { aChipAddr, 0, sizeof(buf), buf };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c[aBusNumber],I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror(">Ioctl error: ");
 		return -1;
 	}

	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}
    return 0;
}

/*------------------------------------------------------------------------------*/

/** write byte on i2c bus
 * @param aChipAddr chip address
 * @param aBuf char value to write
 * @return error return
 */
int 
as_write_byte(int aBusNumber,
              unsigned char aChipAddr,
              unsigned char aValue)
{
   unsigned char buf[1] = {aValue}; // initialise a data buffer with
                                          // address and data

    // create an I2C write message
    struct i2c_msg msg = { aChipAddr, 0, sizeof(buf), buf };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c[aBusNumber], I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		return -1;
 	}

	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}

/*------------------------------------------------------------------------------*/
/** Write several byte on I2C
 * @param aBusNumber i2c bus number
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
as_write_multiple_bytes(int aBusNumber,
                        unsigned char aChipAddr,
                        unsigned char *aBuff,
                        int aSize)
{
    // create an I2C write message
    struct i2c_msg msg = { aChipAddr, 0, aSize, aBuff };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c[aBusNumber], I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		return -1;
 	}

	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}

/*------------------------------------------------------------------------------*/

/** read several bytes on I2C
 * @param aBusNumber i2c bus number
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
as_read_multiple_bytes(int aBusNumber,
                       unsigned char aChipAddr,
                       unsigned char *aBuff,
                       int aSize)
{
    // create an I2C read message
    struct i2c_msg msg = { aChipAddr, 
                           I2C_M_RD, 
                           aSize, 
                           aBuff };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c[aBusNumber], I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 	    return -1;
 	}

	if ( ioctl( mFHandlerI2c[aBusNumber], I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}

/*------------------------------------------------------------------------------*/

/** Close i2c bus files
 * @param aBusNumber i2c bus number
 * @return error code
 */
int 
as_close(int aBusNumber)
{
    return close(mFHandlerI2c[aBusNumber]);
}

