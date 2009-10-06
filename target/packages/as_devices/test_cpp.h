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
#include <iostream>
#include "as_apf27_pwm.hpp"

#define PWM_NUM 0

void pressEnterToContinue(void)
{
    printf("Press enter to continue\n");
    std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
}

void test_pwm(void)
{
    char buffer[20];
    char buffer2[20];
    int value;
    AsApf27Pwm * apf27Pwm;

    apf27Pwm = AsApf27Pwm::getInstance(PWM_NUM);

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("*********************************\n");
        printf("*   Testing  pwm C++ lib menu   *\n");
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
                       apf27Pwm->setFrequency(value);
                       pressEnterToContinue();
                       break;
            case '2' : printf("Current pwm frequency is %d\n",
                              apf27Pwm->getFrequency());
                       pressEnterToContinue();
                       break;
            case '3' : printf("Give period :");
                       scanf("%d",&value);
                       apf27Pwm->setPeriod(value);
                       pressEnterToContinue();
                       break;
            case '4' : printf("Current period is %d\n",
                              apf27Pwm->getPeriod());
                       pressEnterToContinue();
                       break;
            case '5' : printf("Give Duty :");
                       scanf("%d",&value);
                       apf27Pwm->setDuty(value);
                       pressEnterToContinue();
                       break;
            case '6' : printf("Current Duty is %d\n",
                              apf27Pwm->getDuty());
                       pressEnterToContinue();
                       break;
            case '7' : printf("Activate 'a' or Desactivate 'd' ?");
                       scanf("%s",buffer2);
                       if(buffer2[0] == 'a')
                       {
                           apf27Pwm->activate(true);
                           printf("Pwm activated\n");
                           pressEnterToContinue();
                       }else if(buffer2[0] == 'd')
                       {
                           apf27Pwm->activate(false);
                           printf("Pwm desactivated\n");
                           pressEnterToContinue();
                       }else{
                           printf("Option %c unknown\n",buffer[0]);
                           pressEnterToContinue();
                       }
                       break;
            case '8' : if(apf27Pwm->getState())
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
    delete apf27Pwm;
}


