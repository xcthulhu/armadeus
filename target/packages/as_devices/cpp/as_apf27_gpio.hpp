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

#ifndef __ASAPF27GPIO_HPP__
#define __ASAPF27GPIO_HPP__

#define GPIORDDIRECTION	_IOR(PP_IOCTL, 0xF0, int)
#define GPIOWRDIRECTION	_IOW(PP_IOCTL, 0xF1, int)
#define GPIORDDATA	_IOR(PP_IOCTL, 0xF2, int)
#define GPIOWRDATA	_IOW(PP_IOCTL, 0xF3, int) 
#define GPIORDMODE	_IOR(PP_IOCTL, 0xF4, int)
#define GPIOWRMODE	_IOW(PP_IOCTL, 0xF5, int) 

#define GPIO_BASE_PORT ("/dev/gpio/port")

/** AsApf27Gpio description
 *
 */
class AsApf27Gpio {

public:

    static AsApf27Gpio * getInstance(char aPortLetter);

    enum AsApf27GpioError
    {
        AS_APF27_GPIO_FILE_IOCTL_WRITE_MODE_ERROR = -7,
        AS_APF27_GPIO_FILE_IOCTL_READ_MODE_ERROR = -6,
        AS_APF27_GPIO_FILE_IOCTL_READ_DATA_ERROR = -5,
        AS_APF27_GPIO_FILE_IOCTL_WRITE_DIRECTION_ERROR = -4,
        AS_APF27_GPIO_FILE_IOCTL_READ_DIRECTION_ERROR = -3,
        AS_APF27_GPIO_FILE_PATH_TOO_LONG = -2,
        AS_APF27_GPIO_OPEN_FILE_ERROR = -1,
        AS_APF27_GPIO_OK = 0
    };

    virtual ~AsApf27Gpio();

    AsApf27GpioError setPinDirection(int aPinNum,int aDirection);
    AsApf27GpioError setPinValue(int aPinNum, int aValue);
    int getPinValue(int aPinNum);

protected:
    static AsApf27Gpio * mPortA;
    static AsApf27Gpio * mPortB;
    static AsApf27Gpio * mPortC;
    static AsApf27Gpio * mPortD;
    static AsApf27Gpio * mPortE;
    static AsApf27Gpio * mPortF;

    AsApf27Gpio(char aPortLetter);

    AsApf27GpioError init();

    char mPortLetter;

    int mFileHandlerGpioPort;
};

#endif /* __ASAPF27GPIO_HPP__ */


