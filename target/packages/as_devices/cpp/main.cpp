/*
**    THE ARMADEUS PROJECT
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
** Foundation, Inc., 59 Tempe Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <stdio.h>
#include "as_max5821.hpp"
#include "as_apf27_pwm.hpp"

void 
PressEnterToContinue()
{
std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
}


int
main( int argc, const char* argv[] )
{
    AsMax5821 * max5821;
    AsApf27Pwm * pwm0;
    printf("***********************\n");
    printf("* Testing pwm0        *\n");
    printf("***********************\n");
    pwm0 = AsApf27Pwm::getInstance(0);

    pwm0->setDuty(500);
    pwm0->setFrequency(10000);
    pwm0->setPeriod(200);
    pwm0->activate(true);
    printf("pwm0 activated\n");
    printf("Duty %d\n",pwm0->getDuty());
    printf("Frequency %d\n",pwm0->getFrequency());
    printf("Period %d\n",pwm0->getPeriod());
    PressEnterToContinue();
    pwm0->setDuty(250);
    pwm0->setFrequency(12000);
    pwm0->setPeriod(200);
    pwm0->activate(true);
    printf("pwm0 activated\n");
    printf("Duty %d\n",pwm0->getDuty());
    printf("Frequency %d\n",pwm0->getFrequency());
    printf("Period %d\n",pwm0->getPeriod());
    if(pwm0->getState())
        printf("Pwm activated\n");
    else{
        printf("Error pwm is unactive\n");
        exit(0);
    }
    PressEnterToContinue();
    pwm0->activate(false);
    
    printf("***********************\n");
    printf("* Testing max5821     *\n");
    printf("***********************\n");
    max5821 = AsMax5821::getInstance(0);
    printf("Set DAC A to 50%%\n");
    max5821->setDacPercent('A',50);
    PressEnterToContinue();
    printf("Set DAC A to 100%%\n");
    max5821->setDacPercent('A',99);
    PressEnterToContinue();
    printf("Set DAC A to 0%%\n");
    max5821->setDacPercent('A',0);
    printf("End of test\n");
}


