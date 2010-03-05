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
#include <math.h>

#include "as_apf27_pwm.h"
#include "as_93lcxx.h"
#include "as_i2c.h"
#include "as_gpio.h"

#define PWM_NUM 0

void pressEnterToContinue(void)
{
    printf("\nPress enter to continue\n");
    getc(stdin);//XXX 
    while( getc(stdin) != '\n') ; 
}

/* pwm test */
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

/* i2c bus test */
void test_i2c(void)
{
    char buffer[20];
    int value;
    int initialized = 0;

    int i2c_id = 0;
    int i2c_bus = -1;

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
                case '2' :  printf("Give bus number (max %d): ", AS_I2C_DEV_COUNT-1);
                            scanf("%d",&value);
                            if ((value >= AS_I2C_DEV_COUNT) || (value < 0))
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
                            printf("plop\n");
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
    char c_value;
    int32_t value;
    char port_letter = 'F';
    int port_num = 14;
    int port_direction = 0;
    int port_value = 1;

    gpio_dev = as_gpio_open(port_letter);
    if (gpio_dev == NULL)
    {
        printf("Error can't open gpio %c\nHave you run loadgpio.sh ?\n", port_letter);
        pressEnterToContinue();
        return ;
    }
    ret = as_gpio_get_pin_value(gpio_dev,
                                port_num);
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
        printf("   Testing GPIO  P%c%d \n", port_letter, port_num);
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) Change port letter (%c)\n", port_letter);
        printf(" 2) Change port num (%d)\n", port_num);
        printf(" 3) Change direction (%d)\n", port_direction);
        printf(" 4) Change value (%d)\n", port_value);
        printf(" 5) Read pin value\n");

        printf("> ");
        scanf("%s",buffer);

        switch(buffer[0])
        {
            case '1' :  printf("Give letter of port in upper case : ");
                        scanf("%s", buffer);
                        if (buffer[0] != port_letter)
                        {
                            ret = as_gpio_close(gpio_dev);
                            if(ret < 0)
                            {
                                printf("Error, can't close Port%c\n", port_letter);
                                pressEnterToContinue();
                                return ;
                            }
                        }
                        gpio_dev = as_gpio_open(buffer[0]);
                        if(gpio_dev == NULL)
                        {
                            printf("Error, can't open Port%c\n", c_value);
                            pressEnterToContinue();
                            return;
                        }
                        port_letter = buffer[0];
                        printf("Ok Port %c is set\n", port_letter);
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Give pin number : ");
                        scanf("%d", &value);
                        if ((value < 0) || (value > 31))
                            printf("Error, wrong value\n");
                        else
                            port_num = value;
                        pressEnterToContinue();
                        break;
            case '3' :  printf("Give direction (0:in, 1:out) : ");
                        scanf("%d", &value);
                        ret = as_gpio_set_pin_direction(gpio_dev,
                                                        port_num,
                                                        value);
                        if(ret < 0)
                        {
                            printf("Error, can't change direction\n");
                            pressEnterToContinue();
                            return ;
                        }
                        port_direction = value;
                        printf("Ok direction changed\n");
                        pressEnterToContinue();
                        break;
            case '4' :  printf("Give value : ");
                        scanf("%d", &value);
                        ret = as_gpio_set_pin_value(gpio_dev,
                                                    port_num,
                                                    value);
                        if(ret < 0)
                        {
                            printf("Error, can't change pin value\n");
                            pressEnterToContinue();
                            return;
                        }
                        port_value = value;
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '5' :  printf("Get value \n");
                        ret = as_gpio_get_pin_value(gpio_dev,
                                                    port_num);
                        if (ret < 0)
                        {
                            printf("Error, can't get pin value\n");
                            pressEnterToContinue();
                            return;
                        }
                        printf("Value is %d\n",ret);
                        port_value = ret;
                        pressEnterToContinue();
                        break;
                        
            default : break;
        }
    }
}


