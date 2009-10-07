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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  /* for open()   */
#include <errno.h>  /* for perror() */
#include <unistd.h> /* for write()  */

#include "as_apf27_pwm.h"

#define PWM_SYS_PATH   "/sys/class/pwm/pwm"
#define FREQUENCY_PATH "frequency"
#define PERIOD_PATH    "period"
#define DUTY_PATH      "duty" 
#define ACTIVE_PATH    "active"


/* global static variable */
static int mFHandlerFrequency[NUMBER_OF_PWM];
static int mFHandlerPeriod[NUMBER_OF_PWM];
static int mFHandlerDuty[NUMBER_OF_PWM];
static int mFHandlerActive[NUMBER_OF_PWM];
static int mFrequency[NUMBER_OF_PWM];
static int mPeriod[NUMBER_OF_PWM];
static int mDuty[NUMBER_OF_PWM];         
static int mState[NUMBER_OF_PWM];




/*------------------------------------------------------------------------------*/

/** Write a string value in file
 *
 * @param aFile_handler: file handler
 * @param aValueWrite: string value
 * @return error code or number char wrote
 */
int
writeBuffer(int aFile_handler, char *aValueWrite)
{
    int ret;
    ret = write(aFile_handler, aValueWrite, strlen(aValueWrite));
    if (ret < 0)
    {
        perror("Can't write file ");
        return ret;
    }
    ret = lseek(aFile_handler,0,SEEK_SET);
    if (ret < 0)
    {
        perror("lseek error ");
        return ret;
    }
    return ret;
}
/*------------------------------------------------------------------------------*/

/** Initialize pwm
 * @param aPwmNumber number of pwm used
 * @return error code
 */
int
as_apf27_pwm_init(int aPwmNumber)
{
    char buffer[50];

    mState[aPwmNumber] = 0;

    /* open pwm files management */
    snprintf(buffer,50,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,FREQUENCY_PATH); 
    if((mFHandlerFrequency[aPwmNumber] = open(buffer,O_RDWR)) < 0)
    {
        perror("Can't open frequency files : ");
        return mFHandlerPeriod[aPwmNumber];
    }
    snprintf(buffer,50,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,PERIOD_PATH); 
    if((mFHandlerPeriod[aPwmNumber] = open(buffer,O_RDWR)) < 0)
    {
        perror("Can't open period files : ");
        return mFHandlerPeriod[aPwmNumber];
    }
    snprintf(buffer,50,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,DUTY_PATH); 
    if((mFHandlerDuty[aPwmNumber] = open(buffer,O_RDWR)) < 0)
    {
        perror("Can't open duty files : ");
        return mFHandlerDuty[aPwmNumber];
    }
    snprintf(buffer,50,"%s%d/%s",PWM_SYS_PATH,aPwmNumber,ACTIVE_PATH); 
    if((mFHandlerActive[aPwmNumber] = open(buffer,O_RDWR)) < 0)
    {
        perror("Can't open active files : ");
        return mFHandlerActive[aPwmNumber];
    }

    return 0;
}

/*------------------------------------------------------------------------------*/

/** Set pwm frequency
 * @param aPwmNumber number of pwm to close
 * @return no error if positive value
 */
int
as_apf27_pwm_close(int aPwmNumber)
{
    int ret = 0;

    mState[aPwmNumber] = 0;

    /* open pwm files management */
    ret = close(mFHandlerFrequency[aPwmNumber]);
    if(ret < 0)
    {
        perror("Can't close frequency files : ");
        return ret;
    }
    ret = close(mFHandlerPeriod[aPwmNumber]);
    if(ret < 0)
    {
        perror("Can't close period files : ");
        return ret;
    }
    ret = close(mFHandlerDuty[aPwmNumber]);
    if(ret < 0)
    {
        perror("Can't close duty files : ");
        return ret;
    }
    ret = close(mFHandlerActive[aPwmNumber]);
    if(ret < 0)
    {
        perror("Can't close active files : ");
        return ret;
    }

    return ret;

}
/*------------------------------------------------------------------------------*/

/** Set pwm frequency
 * @param aPwmNumber number of pwm used
 * @param aFrequency frequency in Hz
 * @return no error if positive value
 */
int
as_apf27_pwm_setFrequency(int aPwmNumber, int aFrequency)
{
    char buffer[50];

    mFrequency[aPwmNumber] = aFrequency;
    snprintf(buffer,50,"%d",mFrequency[aPwmNumber]);
    return writeBuffer(mFHandlerFrequency[aPwmNumber],buffer);
}

/*------------------------------------------------------------------------------*/
/** get Frequency value
 * @param aPwmNumber number of pwm used
 * @return int frequency in Hz
 */
int 
as_apf27_pwm_getFrequency(int aPwmNumber)
{
    return mFrequency[aPwmNumber];
}

/*------------------------------------------------------------------------------*/
/** set period
 *
 * @param pwm number used
 * @param aPeriod period in us
 * @return positive value if no error
 */
int
as_apf27_pwm_setPeriod(int aPwmNumber, int aPeriod)
{
    char buffer[50];
    mPeriod[aPwmNumber] = aPeriod;
    snprintf(buffer,50,"%d",mPeriod[aPwmNumber]);
    return writeBuffer(mFHandlerPeriod[aPwmNumber],buffer);
}

/*------------------------------------------------------------------------------*/
/** get Period
 * @param aPwmNumber pwm number used
 * @return period int in us
 */
int  
as_apf27_pwm_getPeriod(int aPwmNumber)
{
    return mPeriod[aPwmNumber];
}

/*------------------------------------------------------------------------------*/
/** set duty
 * @param aPwmNumber pwm number used
 * @param aDuty duty in 1/1000
 * @return positive value if no error
 */
int 
as_apf27_pwm_setDuty(int aPwmNumber, int aDuty)
{
    char buffer[50];
    mDuty[aPwmNumber] = aDuty;
    snprintf(buffer, 50, "%d", aDuty);
    return writeBuffer(mFHandlerDuty[aPwmNumber], buffer);
}

/*------------------------------------------------------------------------------*/
/** get duty
 *
 * @param aPwmNumber pwm number used
 * @return duty
 */
int  
as_apf27_pwm_getDuty(int aPwmNumber)
{
    return mDuty[aPwmNumber];
}

/*------------------------------------------------------------------------------*/
/** activate pwm
 * @param aPwmNumber pwm number used
 * @param aEnable
 */
int as_apf27_pwm_activate(int aPwmNumber, int aEnable)
{
    char one[] = "1";
    char zero[] = "0";
    int ret=0;

    if(aEnable)
    {
        ret = writeBuffer(mFHandlerActive[aPwmNumber],one);
    }else{
        ret = writeBuffer(mFHandlerActive[aPwmNumber],zero);
    }
    if (ret < 0)
    {
        return ret;
    }
    mState[aPwmNumber] = aEnable;
    return mState[aPwmNumber];

}

int as_apf27_pwm_getState(int aPwmNumber)
{
    return mState[aPwmNumber];
}


