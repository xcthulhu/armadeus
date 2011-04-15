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
#include <limits>

#include "as_i2c.hpp"
#include "as_gpio.hpp"
#include "as_adc.hpp"
#include "as_dac.hpp"
#include "as_spi.hpp"

#define _STR(x) #x

void pressEnterToContinue(void)
{
    printf("Press enter to continue\n");
    getc(stdin);//XXX
    std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
}


/* i2c bus test */
void test_i2c(void)
{
    char buffer[20];
    int value;
    int initialized = 0;

    AsI2c *i2c_bus = NULL;
    int i2c_id = 0;

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("*********************************\n");
        printf("*   Testing i2c menu            *\n");
        printf("*********************************\n");
        printf("Choose ('q' to quit):\n");
        if (initialized == 0)
        {
            printf(" 1) Open i2c bus \n");
            printf(" 2) Change bus number (%d)\n",i2c_id);
        } else {
            printf(" 1) Close i2c bus \n");
        }
        printf("> ");
        scanf("%s",buffer);

        if (initialized == 0)
        {
            switch(buffer[0])
            {
                case '1' :  printf("Open i2c bus\n");
                            i2c_bus = new AsI2c(i2c_id);
                            if (i2c_bus == NULL)
                            {
                                printf("Error can't open i2c bus\n");
                            } else {
                                printf("Bus %d opened\n", i2c_id);
                                initialized = 1;
                            }
                            pressEnterToContinue();
                            break;
                case '2' :  printf("Give bus number (max %d): ", sizeof(_STR(AS_I2C_DEV_COUNT))-1);
                            scanf("%d",&value);
                            if ((value >= (signed int)sizeof(_STR(AS_I2C_DEV_COUNT))) || (value < 0))
                            {
                                printf(" Wrong value\n");
                            } else {
                                i2c_id = value;
                            }
                            pressEnterToContinue();
                default : break;
            }
        } else {
            switch(buffer[0])
            {
                case '1' :  printf("Close i2c bus\n");
                            delete i2c_bus;
                            initialized = 0;

                            pressEnterToContinue();
                            break;
                default : break;
            }
        }
    }
}

/* spi bus test */
void test_spi(void)
{
    printf("TODO\n");
    pressEnterToContinue();
}

void test_gpio()
{
    char buffer[50];
    AsGpio *gpio_dev = NULL;
    int32_t ret;
    int32_t value;
    char port_letter = 'F';
    int pin_num = 13;
    int port_direction = 0;
    int port_value = 1;

    gpio_dev = new AsGpio(port_letter, pin_num);
    if (gpio_dev == NULL)
    {
        printf("Error can't open gpio %c\nHave you run loadgpio.sh ?\n", port_letter);
        pressEnterToContinue();
        return ;
    }
    ret = gpio_dev->getPinValue();
    if (ret < 0)
    {
        printf("Error, can't get pin value\n");
        return;
    }
    port_value = ret;


    while(buffer[0] != 'q')
    {
        system("clear");
        printf("**************************\n");
        printf("   Testing GPIO  P%c%d \n", gpio_dev->getPortLetter(), gpio_dev->getPinNum());
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) Change gpio (P%c%d)\n", gpio_dev->getPortLetter(), gpio_dev->getPinNum());
        printf(" 2) Change direction (%d)\n", gpio_dev->getPinDirection());
        printf(" 3) Change value (%d)\n", gpio_dev->getPinValue());
        printf(" 4) Read pin value\n");
        printf(" 5) Set irq mode (%d)\n", gpio_dev->getIrqMode());
        printf(" a) blocking read\n");

        printf("> ");
        scanf("%s",buffer);

        switch(buffer[0])
        {
            case '1' :  printf("Give letter of port in upper case : ");
                        scanf("%s", buffer);
                        if (buffer[0] != port_letter)
                        {
                            delete gpio_dev;
                        }
                        port_letter = buffer[0];

                        printf("Give pin number : ");
                        scanf("%d", &pin_num);
                        
    			gpio_dev = new AsGpio(port_letter, pin_num);
                        if(gpio_dev == NULL)
                        {
                            printf("Error, can't open P%c%d\n", port_letter, pin_num);
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok P%c%d is open\n", port_letter, pin_num);
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Give direction (0:in, 1:out) : ");
                        scanf("%d", &value);
                        ret = gpio_dev->setPinDirection(value);
                        if(ret < 0)
                        {
                            printf("Error, can't change direction\n");
                            pressEnterToContinue();
                            break;
                        }
                        port_direction = value;
                        printf("Ok direction changed\n");
                        pressEnterToContinue();
                        break;
            case '3' :  printf("Give value : ");
                        scanf("%d", &value);
                        ret = gpio_dev->setPinValue(value);
                        if(ret < 0)
                        {
                            printf("Error, can't change pin value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '4' :  printf("Get value \n");
                        ret = gpio_dev->getPinValue();
                        if (ret < 0)
                        {
                            printf("Error, can't get pin value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Value is %d\n", ret);
                        port_value = ret;
                        pressEnterToContinue();
                        break;
            case '5' :  printf("1)  GPIO_IRQ_MODE_NOINT  \n");
                        printf("2)  GPIO_IRQ_MODE_RISING \n");
                        printf("3)  GPIO_IRQ_MODE_FALLING\n");
                        printf("4)  GPIO_IRQ_MODE_BOTH   \n");
                        printf("Give value : ");
                        scanf("%d", &value);
                        if (value == 1)value = GPIO_IRQ_MODE_NOINT  ;
                        if (value == 2)value = GPIO_IRQ_MODE_RISING ;
                        if (value == 3)value = GPIO_IRQ_MODE_FALLING;
                        if (value == 4)value = GPIO_IRQ_MODE_BOTH   ;
                        ret = gpio_dev->setIrqMode(value);
                        if(ret < 0)
                        {
                            printf("Error, can't change irq value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case 'a' :  printf("Blocking read \n");
                        ret = gpio_dev->blockingGetPinValue(10, 0);
                        if (ret < 0)
                        {
                            printf("Error, can't read value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Value read %d\n",ret);
                        pressEnterToContinue();
                        break;
            default : break;
        }
    }
    delete gpio_dev;
}


#define NUM_OF_CHANNEL (8)

void test_max1027()
{
    char buffer[50];
    int ret;
    int value;
    float fValue;
    AsAdc *max1027_dev = NULL;
    int channel=0;
    float vRef = 2500;
    int devNumber = 0;
    int initialized = 0;

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("**************************\n");
        printf("   Testing max1027       *\n");
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
	
	if (initialized == 0)
	{
	        printf(" 1) Open Max 1027 device\n");
		printf(" 2) Change device number (%i)\n", devNumber);
	}
	else
	{
		printf(" 1) Close Max 1027 device\n");
		printf(" 2) Change Vref (%fmV)\n",vRef);
		printf(" 3) Select channel (%i)\n", channel);
		printf(" 4) Read channel value\n");
	}

        printf("> ");
        scanf("%s",buffer);

	if (initialized == 0)
	{
            switch(buffer[0])
            {
                case '1' :  printf("Open Max 1027 device\n");
       			    max1027_dev = new AsAdc(AS_MAX1027_NAME, devNumber, vRef);
                            if (max1027_dev < 0)
                            {
                                printf("Error, can't open max1027. Is max1027 modprobed ?\n");
                            } else {
                                printf("Max 1027 device opened\n");
                                initialized = 1;
                            }
                            pressEnterToContinue();
                            break;
                case '2' :  printf("Give device number:");
                            scanf("%i",&value);

			    if (value < 0)
			    {
                                printf(" Wrong value\n");
                            } else {
                                devNumber = value;
                            }
                            pressEnterToContinue();
                default : break;
	    }
	}
	else
	{
            switch(buffer[0])
            {
                    case '1' :  printf("Close Max 1027 device\n");
                                delete max1027_dev;
                                initialized = 0;
                                pressEnterToContinue();
                                break;
		    case '2' :  printf("Give wanted Vref (mV): ");
		                scanf("%f",&fValue);
		                delete max1027_dev;
			
		                max1027_dev = new AsAdc(AS_MAX1027_NAME, devNumber, fValue);
		                if (max1027_dev == NULL){
		                    printf("Error, can't open max1027 with Vref = %fmV\n", fValue);
		                    pressEnterToContinue();
		                    break;
		                }

				vRef = fValue;
		                printf("Vref changed to: %fmV\n", vRef);
		                pressEnterToContinue();
		                break;
		    case '3' :  printf("Give channel you want to read (0-6): ");
		                scanf("%d",&value);
		                if ((value >= NUM_OF_CHANNEL) || (value < 0)){
		                    printf("Channel num wrong\n");
		                    break;
		                }
		                channel = value;
		                break;
		    case '4' :  ret = max1027_dev->getValueInMillivolts(channel);
		                if (ret < 0) {
		                    printf("Error reading temperature\n");
		                    pressEnterToContinue();
		                    break;
		                }
		                printf("Channel %d value read : %dmV\n", channel, ret);
		                pressEnterToContinue();
		                break;
		    default : break;
	     }
        }
    }
}

void test_max5821()
{
    char buffer[50];
    int ret;
    int value;
    AsDac *max5821_dev = NULL;
    int channelA=0;
    int channelB=0;
    float vRef = 2500;


    max5821_dev = new AsDac(AS_MAX5821_TYPE, 0, 0x38, vRef);
    if (max5821_dev == NULL)
    {
        printf("Error, can't open max5821.\n");
        pressEnterToContinue();
        return ;
    }
    pressEnterToContinue();

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("**************************\n");
        printf("   Testing max5821       *\n");
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) select channel A value %d\n", channelA);
        printf(" 2) select channel B value %d\n", channelB);
        printf(" 3) select both channel values\n");

        printf("> ");
        scanf("%s",buffer);

        switch(buffer[0])
        {
            case '1' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = max5821_dev->setValueInMillivolts(0, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select value\n");
                        }
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = max5821_dev->setValueInMillivolts(1, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select value\n");
                        }
                        pressEnterToContinue();
                        break;
            case '3' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = max5821_dev->setValueInMillivolts(0, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select channel 'A' value\n");
                        }
			else
			{
				ret = max5821_dev->setValueInMillivolts(1, value);
		                if (ret < 0)
		                {
		                    printf("Error, can't select channel 'B' value\n");
		                }	
			}			
                        pressEnterToContinue();
                        break;
            default : break;
        }
    }

    delete max5821_dev;
}

/* PWM test */
void test_pwm(void)
{
    printf("TODO\n");
    pressEnterToContinue();
}

/* EEPROM test */
void test_93LC(void)
{
    printf("TODO\n");
    pressEnterToContinue();
}

/* AS1531 test */
void test_as1531(void)
{
    printf("TODO\n");
    pressEnterToContinue();
}

