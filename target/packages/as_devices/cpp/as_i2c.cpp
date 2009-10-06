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
#include "as_i2c.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

AsI2c * AsI2c::mI2c0 = NULL;
AsI2c * AsI2c::mI2c1 = NULL;

/** Singleton constructor
 * @param aBusNumber bus number
 */
AsI2c *
AsI2c::getInstance(int aBusNumber)
{
    if(aBusNumber == 0)
    {
        if(mI2c0 == NULL)
        {
            mI2c0 = new AsI2c(aBusNumber);
        }
        return mI2c0;
    }
    if(aBusNumber == 1)
    {
        if(mI2c1 == NULL)
        {
            mI2c1 = new AsI2c(aBusNumber);
        }
        return mI2c1;
    }
    return NULL;
}

/*------------------------------------------------------------------------------*/

AsI2c::AsI2c(int aBusNumber)
{
    char buffer[40];
    /* make path */
    snprintf(buffer,39,"%s%d",I2C_DEV_PATH,aBusNumber);
    /* open i2c /dev */
    if((mFHandlerI2c = open(buffer,O_RDWR))<0)
    {
        perror("Open error : ");
        exit(1);
    }
    mI2cBusNumber = aBusNumber;
}
/*------------------------------------------------------------------------------*/

AsI2c::~AsI2c()
{
    if(mI2cBusNumber==0)
    {
        mI2c0 = NULL;
    }
    if(mI2cBusNumber==1)
    {
        mI2c1 = NULL;
    }
}

/*------------------------------------------------------------------------------*/
/** read byte on i2c bus
 * @param aChipAddr chip address
 * @param aReg Register number
 * @param *aBuf char buffer
 * @return error return
 */
int 
AsI2c::readByteData(unsigned char aChipAddr, 
                       unsigned char aReg, 
                       unsigned char *aBuf )
{
  // create an I2C write message (only one byte: the address)
    struct i2c_msg msg = { aChipAddr, 0, 1, aBuf };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    aBuf[0] = aReg; // select aReg to read

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c, I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		exit (1);
 	}

    // write the desired aRegister address
	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}
    msg.flags = I2C_M_RD; // read

    // read the result and write it in aBuf[0]
	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
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
AsI2c::writeByteData(unsigned char aChipAddr,
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
    if ( ioctl(mFHandlerI2c,I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror(">Ioctl error: ");
 		exit (1);
 	}

	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
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
AsI2c::writeByte(unsigned char aChipAddr,
                 unsigned char aValue)
{
    unsigned char buf[1] = {aValue}; // initialise a data buffer with
                                          // address and data

    // create an I2C write message
    struct i2c_msg msg = { aChipAddr, 0, sizeof(buf), buf };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c, I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		exit (1);
 	}

	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}

/*------------------------------------------------------------------------------*/
/** Write several byte on I2C
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
AsI2c::writeMultipleBytes(unsigned char aChipAddr,
                          unsigned char *aBuff,
                          int aSize)
{

    // create an I2C write message
    struct i2c_msg msg = { aChipAddr, 0, aSize, aBuff };
    // create a I2C IOCTL request
    struct i2c_rdwr_ioctl_data rdwr = { &msg, 1 };

    /* configure I2C slave */
    if ( ioctl(mFHandlerI2c, I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		exit (1);
 	}

	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}

/*------------------------------------------------------------------------------*/
/** read several bytes on I2C
 * @param aChipAddr Chip address
 * @param aBuff message
 * @param aSize size of message
 * @return error code
 */
int 
AsI2c::readMultipleBytes(unsigned char aChipAddr,
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
    if ( ioctl(mFHandlerI2c, I2C_SLAVE_FORCE, aChipAddr) < 0){
 		perror("Ioctl error: ");
 		exit (1);
 	}

	if ( ioctl( mFHandlerI2c, I2C_RDWR, &rdwr ) < 0 ){
		printf("Write error\n");
		return -1;
	}

    return 0;
}


