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
*/
#include "as_apf27_gpio.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/ppdev.h> 

AsApf27Gpio * AsApf27Gpio::mPortA = NULL;
AsApf27Gpio * AsApf27Gpio::mPortB = NULL;
AsApf27Gpio * AsApf27Gpio::mPortC = NULL;
AsApf27Gpio * AsApf27Gpio::mPortD = NULL;
AsApf27Gpio * AsApf27Gpio::mPortE = NULL;
AsApf27Gpio * AsApf27Gpio::mPortF = NULL;

/*------------------------------------------------------------------------------*/
/** Singleton constructor
 * @param aPortLetter port letter in upper case
 * @return object
 */
AsApf27Gpio *
AsApf27Gpio::getInstance(char aPortLetter)
{
    switch(aPortLetter)
    {
        case 'A':
            if(mPortA == NULL)
            {
                mPortA = new AsApf27Gpio('A');
                mPortA->init();
            }
            return mPortA;
            break;
        case 'B':
            if(mPortB == NULL)
            {
                mPortB = new AsApf27Gpio('B');
                mPortB->init();
            }
            return mPortB;
            break;
        case 'C':
            if(mPortC == NULL)
            {
                mPortC = new AsApf27Gpio('C');
                mPortC->init();
            }
            return mPortC;
            break;
        case 'D':
            if(mPortD == NULL)
            {
                mPortD = new AsApf27Gpio('D');
                mPortD->init();
            }
            return mPortD;
            break;
        case 'E':
            if(mPortE == NULL)
            {
                mPortE = new AsApf27Gpio('E');
                mPortE->init();
            }
            return mPortE;
            break;
        case 'F':
            if(mPortF == NULL)
            {
                mPortF = new AsApf27Gpio('F');
                mPortF->init();
            }
            return mPortF;
            break;
        default:
            return NULL;
            break;
    }
}

/*------------------------------------------------------------------------------*/

AsApf27Gpio::AsApf27Gpio(char aPortLetter)
{
    mPortLetter = aPortLetter;
}
/*------------------------------------------------------------------------------*/

AsApf27Gpio::~AsApf27Gpio()
{
    switch(mPortLetter)
    {
        case 'A':
            mPortA = NULL;
            break;
        case 'B':
            mPortB = NULL;
            break;
        case 'C':
            mPortC = NULL;
            break;
        case 'D':
            mPortD = NULL;
            break;
        case 'E':
            mPortE = NULL;
            break;
        case 'F':
            mPortF = NULL;
            break;
    }
    close(mFileHandlerGpioPort);
}

/*------------------------------------------------------------------------------*/

/** Init Gpio
 * @return AsApf27GpioError
 */

AsApf27Gpio::AsApf27GpioError
AsApf27Gpio::init()
{
    char gpio_file_path[50];
    int ret=0;
    /* make gpio port string path */
    ret = snprintf(gpio_file_path,50,"%s%c",
                            GPIO_BASE_PORT,mPortLetter);

    if(ret < 0)return AS_APF27_GPIO_FILE_PATH_TOO_LONG;

    /* opening gpio port */
    mFileHandlerGpioPort = open(gpio_file_path,O_RDWR);
    if(mFileHandlerGpioPort < 0)return AS_APF27_GPIO_OPEN_FILE_ERROR;

    return AS_APF27_GPIO_OK;
}

/*------------------------------------------------------------------------------*/

/** Set pin direction
 * @param aPinNum : pin number
 * @param aDirection : direction 0:input 1:output
 * @return AsApf27GpioError
 */

AsApf27Gpio::AsApf27GpioError
AsApf27Gpio::setPinDirection(int aPinNum, int aDirection)
{
    int ret=0;
    int portval;

	/* Set LED PIN as GPIO; read/modify/write */
	ret = ioctl(mFileHandlerGpioPort, GPIORDMODE, &portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_READ_MODE_ERROR;

	portval |= (1 << aPinNum);

	ret = ioctl(mFileHandlerGpioPort, GPIOWRMODE, &portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_WRITE_MODE_ERROR;

    /* set direction */
    ret = ioctl(mFileHandlerGpioPort, GPIORDDIRECTION,&portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_READ_DIRECTION_ERROR;
    if(aDirection==0)
    {
        portval &= ~(1 << aPinNum);
    }else{
        portval |= (1 << aPinNum);
    }
    ret = ioctl(mFileHandlerGpioPort, GPIOWRDIRECTION,&portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_WRITE_DIRECTION_ERROR;
    return AS_APF27_GPIO_OK;
}

/*------------------------------------------------------------------------------*/

/** Set pin value 
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 * @return AsApf27GpioError
 */

AsApf27Gpio::AsApf27GpioError
AsApf27Gpio::setPinValue(int aPinNum, int aValue)
{
    int ret=0;
    int portval;
    ret = ioctl(mFileHandlerGpioPort,GPIORDDATA,&portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_READ_DATA_ERROR;
    if(aValue == 0)
    {
        portval &= ~(1 << aPinNum);
    }else{
        portval |= (1 << aPinNum);
    }
    ret = ioctl(mFileHandlerGpioPort,GPIOWRDATA,&portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_READ_DATA_ERROR;

    return AS_APF27_GPIO_OK;
}

/*------------------------------------------------------------------------------*/

/** Set pin value 
 * @param aPinNum pin number
 * @param aValue value of pin (1 or 0)
 * @return AsApf27GpioError
 */

int
AsApf27Gpio::getPinValue(int aPinNum)
{
    int ret=0;
    int portval;
    ret = ioctl(mFileHandlerGpioPort,GPIORDDATA,&portval);
    if(ret < 0)return AS_APF27_GPIO_FILE_IOCTL_READ_DATA_ERROR;

    if( (portval & (1<<aPinNum)) != 0)
    {
        return 1;
    }else{
        return 0;
    }
}
