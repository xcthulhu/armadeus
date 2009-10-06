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

#ifndef __ASAPF27PWM_H__
#define __ASAPF27PWM_H__

#define PWM_SYS_PATH   "/sys/class/pwm/pwm"
#define FREQUENCY_PATH "frequency"
#define PERIOD_PATH    "period"
#define DUTY_PATH      "duty" 
#define ACTIVE_PATH    "active"

/* number of pwm under system */
#define NUMBER_OF_PWM 2


int as_apf27_pwm_init(int aPwmNumber);

int as_apf27_pwm_setFrequency(int aPwmNumber, int aFrequency);
int as_apf27_pwm_getFrequency(int aPwmNumber);

int as_apf27_pwm_setPeriod(int aPwmNumber, int aPeriod);
int  as_apf27_pwm_getPeriod(int aPwmNumber);

int as_apf27_pwm_setDuty(int aPwmNumber, int aDuty);
int  as_apf27_pwm_getDuty(int aPwmNumber);

int as_apf27_pwm_activate(int aPwmNumber, int aEnable);
int as_apf27_pwm_getState(int aPwmNumber);

int as_apf27_pwm_close(int aPwmNumber);

#endif /* __ASAPF27PWM_H__ */
