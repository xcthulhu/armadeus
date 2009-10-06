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

#ifndef __ASAPF27PWM_HPP__
#define __ASAPF27PWM_HPP__

#include "as_apf27_pwm.h"

/** AsApf27Pwm description
 *
 */
class AsApf27Pwm {
public:
    static AsApf27Pwm * getInstance(int aPwmNumber);

    void setFrequency(int aFrequency);
    int  getFrequency(void);

    void setPeriod(int aPeriod);
    int  getPeriod(void);

    void setDuty(int aDuty);
    int  getDuty(void);

    bool activate(bool aEnable);
    bool getState(void);

    virtual ~AsApf27Pwm();

protected:

    AsApf27Pwm(int aPwmNumber);

    static AsApf27Pwm * mPwm0;
    static AsApf27Pwm * mPwm1;

    int mPwmNumber;

};

#endif /* __ASAPF27PWM_HPP__ */


