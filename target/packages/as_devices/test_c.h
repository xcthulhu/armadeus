/*
 **
 **    Copyright (C) 2009-2011  The Armadeus Project - ARMadeus Systems
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
 **** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "as_pwm.h"
#include "as_93lcxx.h"
#include "as_i2c.h"
#include "as_gpio.h"
#include "as_max1027.h"
#include "as_max5821.h"
#include "as_adc.h"
#include "as_dac.h"
#include "as_helpers.h"

#define PWM_NUM 0

#define _STR(x) #x

void pressEnterToContinue(void)
{
    printf("\nPress enter to continue\n");
    while( getc(stdin) != '\n');
    while( getc(stdin) != '\n');
}

/* pwm test */
void test_pwm(void)
{
    int ret;
    char buffer[20];
    char buffer2[20];
    int value;
    struct as_pwm_device *pwm_dev;

    pwm_dev = as_pwm_open(PWM_NUM);
    if(pwm_dev == NULL){
        printf("can't init pwm0\n");
        pressEnterToContinue();
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
                       as_pwm_set_frequency(pwm_dev,value);
                       pressEnterToContinue();
                       break;
            case '2' : printf("Current pwm frequency is %d\n",
                              as_pwm_get_frequency(pwm_dev));
                       pressEnterToContinue();
                       break;
            case '3' : printf("Give period :");
                       scanf("%d",&value);
                       as_pwm_set_period(pwm_dev,value);
                       pressEnterToContinue();
                       break;
            case '4' : printf("Current period is %d\n",
                              as_pwm_get_period(pwm_dev));
                       pressEnterToContinue();
                       break;
            case '5' : printf("Give Duty :");
                       scanf("%d",&value);
                       as_pwm_set_duty(pwm_dev,value);
                       pressEnterToContinue();
                       break;
            case '6' : printf("Current Duty is %d\n",
                              as_pwm_get_duty(pwm_dev));
                       pressEnterToContinue();
                       break;
            case '7' : printf("Activate 'a' or Desactivate 'd' ?");
                       scanf("%s",buffer2);
                       if(buffer2[0] == 'a')
                       {
                           as_pwm_set_state(pwm_dev,1);
                           printf("Pwm activated\n");
                           pressEnterToContinue();
                       }else if(buffer2[0] == 'd')
                       {
                           as_pwm_set_state(pwm_dev,0);
                           printf("Pwm desactivated\n");
                           pressEnterToContinue();
                       }else{
                           printf("Option %c unknown\n",buffer[0]);
                           pressEnterToContinue();
                       }
                       break;
            case '8' : if(as_pwm_get_state(pwm_dev))
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
    ret = as_pwm_close(pwm_dev);
    if(ret < 0){
        printf("can't close pwm0\n");
        return;
    }
}

/* i2c bus test */
void test_i2c(void)
{
    char buffer[20];
    int value;
    int initialized = 0;

    struct as_i2c_device *i2c_bus = 0;
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
                            i2c_bus = as_i2c_open( i2c_id);
                            if (i2c_bus < 0)
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
                            if ((value >= sizeof(_STR(AS_I2C_DEV_COUNT))) || (value < 0))
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
                            value = as_i2c_close(i2c_bus);
                            if (value < 0)
                            {
                                printf(" Error, can't close i2c bus num %d\n", i2c_id);
                            } else {
                                initialized = 0;
                            }
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


/* 93LCxx eepromm test */
int address_size(int aType, int aWordSize)
{
    return (((aType-46)/10) + 6 + 2 - (aWordSize/8));
}

int max_address(int aType, int aWordSize)
{
    return (pow(2,address_size(aType, aWordSize))-1);
}

void test_93LC()
{
    char buffer[50];
    int i;

    int type = 46;
    char spidevpath[50];
    struct as_93lcxx_device *dev;
    /* defaults values */
    int frequency = 1000000;
    uint8_t word_size = 8;
    int value, value2;
    int initialized=0;
    snprintf(spidevpath, 50, "%s", "/dev/spidev2.0");

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("*********************************\n");
        printf("*   Testing 93LCxx menu         *\n");
        printf("*********************************\n");
        printf("Choose ('q' to quit):\n");
        if (initialized == 0)
        {
            printf(" 1) Change word size (%d)\n", word_size);
            printf(" 2) Change bus frequency (%d)\n", frequency);
            printf(" 3) Change eeprom type (%d)\n", type);
            printf(" 4) Change spidev path (%s)\n", spidevpath);
            printf(" 5) Initialize eeprom\n");
        } else {
            printf(" 1) Close eeprom\n");
            printf(" 2) Read specific address\n");
            printf(" 3) Erase specific address\n");
            printf(" 4) Write specific address\n");
            printf(" 5) Write all\n");
            printf(" 6) Read all\n");
            printf(" 7) Erase all\n");
            printf(" 8) Unlock write\n");
            printf(" 9) lock write\n");
        }

        printf("> ");
        scanf("%s",buffer);

        if (initialized == 0)
        {
            switch(buffer[0])
            {
                case '1' :  printf("Give new word size (8 or 16) : ");
                            scanf("%d", &value);
                            if ( (value != 8) && (value != 16) )
                            {
                                printf("Wrong value\n");
                                pressEnterToContinue();
                            } else {
                                word_size = value;
                            }
                            break;
                case '2' :  printf("Give bus frequency : ");
                            scanf("%d", &value);
                            frequency = value;
                            printf("Frequency changed to %dHz\n",frequency);
                            break;
                case '3' :  printf("Give new type of eeprom (46, 56 or 66) : ");
                            scanf("%d", &value);
                            if ( (value != 46) && (value != 56) && (value != 66) )
                            {
                                printf("Wrong value\n");
                                pressEnterToContinue();
                            } else {
                                type = value;
                            }
                            break;
                case '4' :  printf("Give new path for spidev : \n");
                            scanf("%s", spidevpath);
                            printf("New path is %s\n",spidevpath);
                            break;
                case '5' :  dev = as_93lcxx_open((unsigned char *)spidevpath,
                                                 type,
                                                 frequency,
                                                 word_size);
                            if (dev == NULL)
                            {
                                printf("Error, can't initialize eeprom. Have you modprobed spidev ?\n");
                            } else {
                                initialized = 1;
                            }
                            pressEnterToContinue();
                            break;
                default : break;
            }
        } else {
            switch(buffer[0])
            {
                case '1' :  as_93lcxx_close(dev);
                            printf("eeprom closed\n");
                            initialized = 0;
                            pressEnterToContinue();
                            break;
                case '2' :  printf(" Read specific address\n");
                            printf("Give address in hexadecimal (max 0x%03X):",
                                   max_address(type, word_size));
                            scanf("%x",&value);
                            if (value > max_address(type, word_size))
                            {
                                printf(" Error, address wrong\n");
                            } else {
                                value2 = as_93lcxx_read(dev, value);
                                if (value2 < 0)
                                {
                                    printf("Error, can't read value\n");
                                } else {
                                    printf(" Read %02x at %02X", value2, value );
                                }
                            }
                            pressEnterToContinue();
                            break;
                case '3' :  printf("Erase specific address\n");
                            printf("Give address in hexadecimal (max 0x%03X):",
                                   max_address(type, word_size));
                            scanf("%x",&value);
                            if (value > max_address(type, word_size))
                            {
                                printf(" Error, address wrong\n");
                            } else {
                                value2 = as_93lcxx_erase(dev, value);
                                if (value2 < 0)
                                {
                                    printf("Error, can't erase value\n");
                                } else {
                                    printf("Value erased\n");
                                }
                            }
                            pressEnterToContinue();
                            break;
                case '4' :  printf("Write specific address\n");
                            printf("Give address in hexadecimal (max 0x%03X):",
                                   max_address(type, word_size));
                            scanf("%x",&value);
                            if (value > max_address(type, word_size))
                            {
                                printf(" Error, address wrong\n");
                                pressEnterToContinue();
                                break;
                            }
                            printf("Give value in hexadecimal :");
                            scanf("%x",&value2);
                            value2 = as_93lcxx_write(dev, value, value2);
                            if (value2 < 0)
                            {
                                printf("Error, can't write value\n");
                            }
                            pressEnterToContinue();
                            break;
                case '5' :  printf(" Write all\n");
                            printf("Give value in hexadecimal:");
                            scanf("%x",&value);
                            value2 = as_93lcxx_write_all(dev, value);
                            if (value2 < 0)
                            {
                                printf("Error, can't write value\n");
                            }
                            pressEnterToContinue();
                            break;
                case '6' :  printf(" Read all\n");
                            for(i = 0; i <= max_address(type, word_size); i++)
                            {
                                if (word_size == 8)
                                {
                                    printf("%03X -> %02X\n",
                                           i,as_93lcxx_read(dev, i));
                                } else{
                                    printf("%03X -> %04X\n",
                                           i,as_93lcxx_read(dev, i));
                                }
                            }
                            pressEnterToContinue();
                            break;
                case '7' :  printf(" Erase all\n");
                            value = as_93lcxx_erase_all(dev);
                            if (value < 0)
                                printf(" Error in erasing eeprom\n");
                            pressEnterToContinue();
                            break;
                case '8' :  printf("Unlock write\n");
                            if (as_93lcxx_ewen(dev) < 0)
                                printf("Error can't unlock write\n");
                            pressEnterToContinue();
                            break;
                case '9' :  printf("lock write\n");
                            if (as_93lcxx_ewds(dev) < 0)
                                printf("Error can't lock write\n");
                            pressEnterToContinue();
                            break;
                default : break;
            }
        }
    }
}

void test_gpio()
{
    char buffer[50];
    struct as_gpio_device *gpio_dev;
    int32_t ret;
    int32_t value;
    char port_letter = 'F';
    int pin_num = 13;
    int port_direction = 0;
    int port_value = 1;
    int platform;

    platform = as_helpers_get_platform();
    switch (platform) {
        case APF9328:
            port_letter = 'X';
            pin_num = 0x007;
        break;
        case APF27:
            port_letter = 'F';
            pin_num = 13;
        break;
        case APF51:
            port_letter = 'A';
            pin_num = 3;
        break;
        default:
        break;
    }
    gpio_dev = as_gpio_open(port_letter, pin_num);
    if (gpio_dev == NULL)
    {
        printf("Error can't open gpio %c\nHave you run loadgpio.sh ?\n", port_letter);
        pressEnterToContinue();
        return ;
    }
    ret = as_gpio_get_pin_value(gpio_dev);
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
        printf("   Testing GPIO  Port%c%d \n", port_letter, pin_num);
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) Change gpio (P%c%d)\n", as_gpio_get_port_letter(gpio_dev), 
                                            as_gpio_get_pin_num(gpio_dev));
        printf(" 2) Change direction (%s)\n", as_gpio_get_pin_direction(gpio_dev));
        printf(" 3) Change value (%d)\n", as_gpio_get_pin_value(gpio_dev));
        printf(" 4) Read pin value\n");
        printf(" 5) Set irq/event mode (%s)\n", as_gpio_get_irq_mode(gpio_dev));
        printf(" a) Wait an event\n");

        printf("> ");
        scanf("%s",buffer);

        switch (buffer[0])
        {
            case '1' :  printf("Give letter of port in upper case : ");
                        scanf("%s", buffer);
                        if (buffer[0] != port_letter)
                        {
                            ret = as_gpio_close(gpio_dev);
                            if(ret < 0)
                            {
                                printf("Error, can't close gpio\n");
                                pressEnterToContinue();
                                break;
                            }
                        }
                        port_letter = buffer[0];

                        printf("Give pin number : ");
                        scanf("%d", &pin_num);
                        
                        gpio_dev = as_gpio_open(port_letter,pin_num);
                        if (gpio_dev == NULL)
                        {
                            printf("Error, can't open P%c%d\n", port_letter, pin_num);
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok P%c%d is open\n", port_letter, pin_num);
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Give direction (in/out) : ");
                        scanf("%s", buffer);
                        ret = as_gpio_set_pin_direction(gpio_dev, buffer);
                        if (ret < 0)
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
                        ret = as_gpio_set_pin_value(gpio_dev, value);
                        if (ret < 0)
                        {
                            printf("Error, can't change pin value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '4' :  printf("Get value \n");
                        ret = as_gpio_get_pin_value(gpio_dev);
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
            case '5':
			printf("rising / falling / both / none\n");
                        printf("Please choose a mode : ");
                        scanf("%s", buffer);
                        ret = as_gpio_set_irq_mode(gpio_dev, buffer);
                        if (ret < 0)
                        {
                            printf("Error, can't change irq value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case 'a' :  printf("Blocking read (10 sec timeout)\n");
                        ret = as_gpio_wait_event(gpio_dev, 10000);
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
    ret = as_gpio_close(gpio_dev);
    if(ret < 0)
    {
        printf("Error, can't close gpio\n");
        pressEnterToContinue();
    }
}

#define NUM_OF_CHANNEL (8)

void test_max1027()
{
    char buffer[50];
    int ret;
    int value;
    float fValue;
    struct as_adc_device *max1027_dev;
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
                case '1' :  printf("Open MAX1027 device\n");
                            max1027_dev = as_adc_open(AS_MAX1027_NAME, devNumber, vRef);
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
                    case '1' :  printf("Close MAX1027 device\n");
                            ret = as_adc_close(max1027_dev);
                            if (ret < 0)
                            {
                                printf(" Error, can't close Max 1027 device\n");
                            } else {
                                initialized = 0;
                            }
                            pressEnterToContinue();
                            break;
		    case '2' :  printf("Give wanted Vref (mV): ");
		                scanf("%f",&fValue);
		                ret = as_adc_close(max1027_dev);
				if (ret < 0)
				{
				    printf("Error, can't close max1027\n");
				    pressEnterToContinue();
				}
			
		                max1027_dev = as_adc_open(AS_MAX1027_NAME, devNumber, fValue);
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
		    case '4' :  ret = as_adc_get_value_in_millivolts(max1027_dev,
		                                                     channel);
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
    struct as_dac_device *max5821_dev;
    int channelA=0;
    int channelB=0;
    float vRef = 2500;


    max5821_dev = as_dac_open(AS_MAX5821_TYPE, 0, 0x38, vRef);
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
        printf(" 1) select power mode for channel A\n");
        printf(" 2) select power mode for channel B\n");
        printf(" 3) select channel A value %d\n", channelA);
        printf(" 4) select channel B value %d\n", channelB);
        printf(" 5) select both channel values\n");

        printf("> ");
        scanf("%s",buffer);

        switch(buffer[0])
        {
            case '1' :  printf("Choose your mode:\n");
                        printf(" 0) MAX5821_POWER_UP        \n");
                        printf(" 1) MAX5821_POWER_DOWN_MODE0\n");
                        printf(" 2) MAX5821_POWER_DOWN_MODE1\n");
                        printf(" 3) MAX5821_POWER_DOWN_MODE2\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = as_dac_max5821_power(max5821_dev, 'A', value);
                        if (ret < 0)
                        {
                            printf("Error, can't select power mode\n");
                        }
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Choose your mode:\n");
                        printf(" 0) MAX5821_POWER_UP        \n");
                        printf(" 1) MAX5821_POWER_DOWN_MODE0\n");
                        printf(" 2) MAX5821_POWER_DOWN_MODE1\n");
                        printf(" 3) MAX5821_POWER_DOWN_MODE2\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = as_dac_max5821_power(max5821_dev, 'B', value);
                        if (ret < 0)
                        {
                            printf("Error, can't select power mode\n");
                        }
                        pressEnterToContinue();
                        break;
            case '3' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = as_dac_set_value_in_millivolts(max5821_dev, 0, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select value\n");
                        }
                        pressEnterToContinue();
                        break;
            case '4' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = as_dac_set_value_in_millivolts(max5821_dev, 1, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select value\n");
                        }
                        pressEnterToContinue();
                        break;
            case '5' :  printf("Give value between 0-1023\n");
                        printf("> ");
                        scanf("%d", &value);
                        ret = as_max5821_set_both_value(max5821_dev, value);
                        if (ret < 0)
                        {
                            printf("Error, can't select value\n");
                        }
                        pressEnterToContinue();
                        break;
            default : break;
        }
    }

    ret = as_dac_close(max5821_dev);
    if (ret < 0) {
        printf("Error on closing max5821_dev\n");
        pressEnterToContinue();
    }
}

/* AS1531 test */
void test_as1531(void)
{
    printf("TODO\n");
    pressEnterToContinue();
}

