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

#include <stdio.h>
#include <stdlib.h>
#include "as_apf27_pwm.h"

#define PWM_NUM 0

void pressEnterToContinue(void)
{
    printf("Press enter to continue\n");
    getc(stdin);//XXX 
    while( getc(stdin) != '\n') ; 
}

void test_pwm(void)
{
    int ret;
    char buffer[20];
    char buffer2[20];
    int value;

    ret = as_apf27_pwm_init(PWM_NUM);
    if(ret < 0){
        printf("can't init pwm0\n");
        return;
    }
    while(buffer[0] != 'q')
    {
        system("clear");
        printf("*********************************\n");
        printf("*   Testing  pwm menu           *\n");
        printf("*********************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) set Frequency\n");
        printf(" 2) get Frequency\n");
        printf(" 3) set Period\n");
        printf(" 4) get Period\n");
        printf(" 5) set Duty\n");
        printf(" 6) get Duty\n");
        printf(" 7) activate/desactivate\n");
        printf(" 8) get state\n");
        printf("> ");
        scanf("%s",buffer);
        
        switch(buffer[0])
        {
            case '1' : printf("Give frequency :");
                       scanf("%d",&value);
                       as_apf27_pwm_setFrequency(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '2' : printf("Current pwm frequency is %d\n",
                             as_apf27_pwm_getFrequency(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '3' : printf("Give period :");
                       scanf("%d",&value);
                       as_apf27_pwm_setPeriod(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '4' : printf("Current period is %d\n",
                              as_apf27_pwm_getPeriod(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '5' : printf("Give Duty :");
                       scanf("%d",&value);
                       as_apf27_pwm_setDuty(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '6' : printf("Current Duty is %d\n",
                              as_apf27_pwm_getDuty(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '7' : printf("Activate 'a' or Desactivate 'd' ?");
                       scanf("%s",buffer2);
                       if(buffer2[0] == 'a')
                       {
                           as_apf27_pwm_activate(PWM_NUM,1);
                           printf("Pwm activated\n");
                           pressEnterToContinue();
                       }else if(buffer2[0] == 'd')
                       {
                           as_apf27_pwm_activate(PWM_NUM,0);
                           printf("Pwm desactivated\n");
                           pressEnterToContinue();
                       }else{
                           printf("Option %c unknown\n",buffer[0]);
                           pressEnterToContinue();
                       }
                       break;
            case '8' : if(as_apf27_pwm_getState(PWM_NUM))
                       {
                           printf("pwm is active\n");
                           pressEnterToContinue();
                       } else {
                           printf("pwm is inactive\n");
                           pressEnterToContinue();
                       }
                       break;
            default : break;
        }
    }
    ret = as_apf27_pwm_close(PWM_NUM);
    if(ret < 0){
        printf("can't close pwm0\n");
        return;
    }
}


