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
#include "as_apf27_pwm.hpp"
#include <string.h>
#include <fcntl.h>  /* for open()   */
#include <errno.h>  /* for perror() */
#include <unistd.h> /* for write()  */
#include <iostream>

AsApf27DynamicTable * AsApf27Pwm::mInstances = new AsApf27DynamicTable();

/*------------------------------------------------------------------------------*/
AsApf27Pwm * 
AsApf27Pwm::getInstance(int aPwmNumber)
{
        void * instance;

        instance = mInstances->getInstance(aPwmNumber);
        if(instance == NULL)
        {
            instance =(void *)(new AsApf27Pwm(aPwmNumber));
            mInstances->setInstance(instance,aPwmNumber);
        }
        return (AsApf27Pwm *)instance;
}

/*------------------------------------------------------------------------------*/

AsApf27Pwm::AsApf27Pwm(int aPwmNumber)
{
    int ret;
    ret = as_apf27_pwm_init(aPwmNumber);
    if(ret < 0)
    {
        std::cout << "Pwm initialization error" << std::endl;
    }
}

/*------------------------------------------------------------------------------*/

AsApf27Pwm::~AsApf27Pwm()
{
    mInstances->setInstance(NULL,mPwmNumber);
}

/*------------------------------------------------------------------------------*/
/** set Frequency
 *
 * @param aFrequency frequency
 */
void
AsApf27Pwm::setFrequency(int aFrequency)
{
    as_apf27_pwm_setFrequency(mPwmNumber, aFrequency);
}

/*------------------------------------------------------------------------------*/
/** get Frequency value
 *
 * @return int frequency
 */
int
AsApf27Pwm::getFrequency(void)
{
    return as_apf27_pwm_getFrequency(mPwmNumber);
}

/*------------------------------------------------------------------------------*/
/** set period
 *
 * @param aPeriod period
 */
void
AsApf27Pwm::setPeriod(int aPeriod)
{
    as_apf27_pwm_setPeriod(mPwmNumber, aPeriod);
}

/*------------------------------------------------------------------------------*/
/** get Period
 *
 * @return period int
 */
int
AsApf27Pwm::getPeriod(void)
{
    return as_apf27_pwm_getPeriod(mPwmNumber);
}

/*------------------------------------------------------------------------------*/
/** set duty
 * @param aDuty duty
 */
void
AsApf27Pwm::setDuty(int aDuty)
{
    as_apf27_pwm_setDuty(mPwmNumber, aDuty);
}

/*------------------------------------------------------------------------------*/
/** get duty
 *
 * @return duty
 */
int
AsApf27Pwm::getDuty(void)
{
    return as_apf27_pwm_getDuty(mPwmNumber);
}

/*------------------------------------------------------------------------------*/
/** activate pwm
 * @param aEnable
 */
bool
AsApf27Pwm::activate(bool aEnable)
{
    int ret;

    if(aEnable)
        as_apf27_pwm_activate(mPwmNumber,1);
    else
        as_apf27_pwm_activate(mPwmNumber,0);

    if(ret)
        return true;
    else
        return false;
}

/*------------------------------------------------------------------------------*/
/** get State of pwm (enable or not)
 * @return state
 */
bool
AsApf27Pwm::getState(void)
{
    int ret;
    ret = as_apf27_pwm_getState(mPwmNumber);

    if(ret)
        return true;
    else
        return false;
}


