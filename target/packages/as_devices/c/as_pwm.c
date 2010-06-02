/*
**    THE ARMadeus Systems
**
**    Copyright (C) 2009-2010  The armadeus systems team
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  /* for open()   */
#include <unistd.h> /* for write()  */

#include "as_pwm.h"

//#define DEBUG
#ifdef DEBUG
#   define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#   define ERROR(fmt, ...) /*fmt, ##__VA_ARGS__*/
#endif

#define PWM_SYS_PATH   "/sys/class/pwm/pwm"
#define FREQUENCY_PATH "frequency"
#define PERIOD_PATH    "period"
#define DUTY_PATH      "duty"
#define ACTIVE_PATH    "active"

#define SIZE_OF_BUFF 50

/*------------------------------------------------------------------------------*/

/** @brief Write a string value in file
 *
 * @param aFile_handler: file handler
 * @param aValueWrite: string value
 *
 * @return number char wrote, negative value on error
 */
int writeBuffer(int aFile_handler, char *aValueWrite)
{
    int ret;
    ret = write(aFile_handler, aValueWrite, strlen(aValueWrite));
    if (ret < 0)
    {
        ERROR("Can't write file ");
        return ret;
    }
    ret = lseek(aFile_handler,0,SEEK_SET);
    if (ret < 0)
    {
        ERROR("lseek error ");
        return ret;
    }
    return ret;
}

/*------------------------------------------------------------------------------*/

/** @brief read a string value in file
 *
 * @param aFile_handler: file handler
 * @param aValueRead[SIZE_OF_BUFF]: char pointer to read buffer
 *
 * @return number char read, negative value on error
 */
int readBuffer(int aFile_handler, char *aValueRead)
{
    int ret;
    ret = read(aFile_handler, aValueRead, SIZE_OF_BUFF);
    if (ret < 0)
    {
        ERROR("Can't read file ");
        return ret;
    }
    ret = lseek(aFile_handler,0,SEEK_SET);
    if (ret < 0)
    {
        ERROR("lseek error ");
        return ret;
    }
    return ret;
}

/*------------------------------------------------------------------------------*/

struct as_pwm_device *as_pwm_open(int aPwmNumber)
{
    char buffer[SIZE_OF_BUFF];
    struct as_pwm_device *dev;

    dev = (struct as_pwm_device *)malloc(sizeof(struct as_pwm_device));
    if (dev == NULL)
    {
        ERROR("Can't allocate memory for pwm device structure\n");
        return NULL;
    }

    /* open pwm files management */
    snprintf(buffer,SIZE_OF_BUFF,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,FREQUENCY_PATH);
    if ((dev->fileFrequency = open(buffer,O_RDWR)) < 0)
    {
        ERROR("Can't open frequency files");
        return NULL;
    }
    snprintf(buffer,SIZE_OF_BUFF,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,PERIOD_PATH);
    if ((dev->filePeriod = open(buffer,O_RDWR)) < 0)
    {
        ERROR("Can't open period files");
        return NULL;
    }
    snprintf(buffer,SIZE_OF_BUFF,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,DUTY_PATH);
    if((dev->fileDuty = open(buffer,O_RDWR)) < 0)
    {
        ERROR("Can't open duty files");
        return NULL;
    }
    snprintf(buffer,SIZE_OF_BUFF,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,ACTIVE_PATH);
    if((dev->fileActive = open(buffer,O_RDWR)) < 0)
    {
        ERROR("Can't open active files");
        return NULL;
    }

    return dev;
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_set_frequency(struct as_pwm_device *aDev, int aFrequency)
{
    char buffer[SIZE_OF_BUFF];

    snprintf(buffer,SIZE_OF_BUFF,"%d",aFrequency);
    return writeBuffer(aDev->fileFrequency,buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_get_frequency(struct as_pwm_device *aDev)
{
    char buffer[SIZE_OF_BUFF];
    int ret;

    ret = readBuffer(aDev->fileFrequency,  buffer);
    if (ret < 0)
    {
        ERROR("Can't read frequency\n");
        return ret;
    }

    return atoi(buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_set_period(struct as_pwm_device *aDev, int aPeriod)
{
    char buffer[SIZE_OF_BUFF];

    snprintf(buffer,SIZE_OF_BUFF,"%d",aPeriod);
    return writeBuffer(aDev->filePeriod, buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_get_period(struct as_pwm_device *aDev)
{
    char buffer[SIZE_OF_BUFF];
    int ret;

    ret = readBuffer(aDev->filePeriod,  buffer);
    if (ret < 0)
    {
        ERROR("Can't read period\n");
        return ret;
    }

    return atoi(buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_set_duty(struct as_pwm_device *aDev, int aDuty)
{
    char buffer[SIZE_OF_BUFF];

    snprintf(buffer, SIZE_OF_BUFF, "%d", aDuty);
    return writeBuffer( aDev->fileDuty, buffer);
}

/*------------------------------------------------------------------------------*/

int32_t  as_pwm_get_duty(struct as_pwm_device *aDev)
{
    char buffer[SIZE_OF_BUFF];
    int ret;

    ret = readBuffer(aDev->fileDuty,  buffer);
    if (ret < 0)
    {
        ERROR("Can't read duty\n");
        return ret;
    }

    return atoi(buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_set_state(struct as_pwm_device *aDev, int aEnable)
{
    char one[] = "1";
    char zero[] = "0";
    int ret=0;

    if(aEnable)
    {
        ret = writeBuffer(aDev->fileActive,one);
    }else{
        ret = writeBuffer(aDev->fileActive,zero);
    }
    if (ret < 0)
    {
        ERROR("Can't write file Active\n");
        return ret;
    }
    return ret;
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_get_state(struct as_pwm_device *aDev)
{
    char buffer[SIZE_OF_BUFF];
    int ret;

    ret = readBuffer(aDev->fileActive,  buffer);
    if (ret < 0)
    {
        ERROR("Can't read state\n");
        return ret;
    }

    return atoi(buffer);
}

/*------------------------------------------------------------------------------*/

int32_t as_pwm_close(struct as_pwm_device *aDev)
{
    int ret = 0;

    /* open pwm files management */
    ret = close(aDev->fileFrequency);
    if(ret < 0)
    {
        ERROR("Can't close frequency files");
        return ret;
    }
    ret = close(aDev->filePeriod);
    if(ret < 0)
    {
        ERROR("Can't close period files");
        return ret;
    }
    ret = close(aDev->fileDuty);
    if(ret < 0)
    {
        ERROR("Can't close duty files");
        return ret;
    }
    ret = close(aDev->fileActive);
    if(ret < 0)
    {
        ERROR("Can't close active files");
        return ret;
    }

    return ret;

}
