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

#include "as_pwm.h"
#include "as_93lcxx.h"
#include "as_i2c.h"
#include "as_gpio.h"
#include "as_max1027.h"

#define PWM_NUM 0

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

    ret = as_pwm_init(PWM_NUM);
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
                       as_pwm_setFrequency(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '2' : printf("Current pwm frequency is %d\n",
                              as_pwm_getFrequency(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '3' : printf("Give period :");
                       scanf("%d",&value);
                       as_pwm_setPeriod(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '4' : printf("Current period is %d\n",
                              as_pwm_getPeriod(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '5' : printf("Give Duty :");
                       scanf("%d",&value);
                       as_pwm_setDuty(PWM_NUM,value);
                       pressEnterToContinue();
                       break;
            case '6' : printf("Current Duty is %d\n",
                              as_pwm_getDuty(PWM_NUM));
                       pressEnterToContinue();
                       break;
            case '7' : printf("Activate 'a' or Desactivate 'd' ?");
                       scanf("%s",buffer2);
                       if(buffer2[0] == 'a')
                       {
                           as_pwm_activate(PWM_NUM,1);
                           printf("Pwm activated\n");
                           pressEnterToContinue();
                       }else if(buffer2[0] == 'd')
                       {
                           as_pwm_activate(PWM_NUM,0);
                           printf("Pwm desactivated\n");
                           pressEnterToContinue();
                       }else{
                           printf("Option %c unknown\n",buffer[0]);
                           pressEnterToContinue();
                       }
                       break;
            case '8' : if(as_pwm_getState(PWM_NUM))
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
    ret = as_pwm_close(PWM_NUM);
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
    int pin_num = 14;
    int port_direction = 0;
    int port_value = 1;
    int pullup=1;

    gpio_dev = as_gpio_open(port_letter);
    if (gpio_dev == NULL)
    {
        printf("Error can't open gpio %c\nHave you run loadgpio.sh ?\n", port_letter);
        pressEnterToContinue();
        return ;
    }
    ret = as_gpio_get_pin_value(gpio_dev,
                                pin_num);
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
        printf("   Testing GPIO  P%c%d \n", port_letter, pin_num);
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) Change port letter (%c)\n", port_letter);
        printf(" 2) Change port num (%d)\n", pin_num);
        printf(" 3) Change direction (%d)\n", port_direction);
        printf(" 4) Change value (%d)\n", port_value);
        printf(" 5) Read pin value\n");
        printf(" 6) Change Pull-Up (%d)\n", pullup);
        printf(" 7) Read Pull-Up\n");
        printf(" 8) Set irq mode\n");
        printf(" 9) get irq mode\n");
        printf(" a) blocking read\n");

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
                                break;
                            }
                        }
                        gpio_dev = as_gpio_open(buffer[0]);
                        if(gpio_dev == NULL)
                        {
                            printf("Error, can't open Port%c\n", c_value);
                            pressEnterToContinue();
                            break;
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
                            pin_num = value;
                        pressEnterToContinue();
                        break;
            case '3' :  printf("Give direction (0:in, 1:out) : ");
                        scanf("%d", &value);
                        ret = as_gpio_set_pin_direction(gpio_dev,
                                                        pin_num,
                                                        value);
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
            case '4' :  printf("Give value : ");
                        scanf("%d", &value);
                        ret = as_gpio_set_pin_value(gpio_dev,
                                                    pin_num,
                                                    value);
                        if(ret < 0)
                        {
                            printf("Error, can't change pin value\n");
                            pressEnterToContinue();
                            break;
                        }
                        port_value = value;
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '5' :  printf("Get value \n");
                        ret = as_gpio_get_pin_value(gpio_dev,
                                                    pin_num);
                        if (ret < 0)
                        {
                            printf("Error, can't get pin value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Value is %d\n",ret);
                        port_value = ret;
                        pressEnterToContinue();
                        break;
            case '6' :  printf("Give value : ");
                        scanf("%d", &value);
                        ret = as_gpio_set_pullup_value(gpio_dev,
                                                       pin_num,
                                                       value);
                        if(ret < 0)
                        {
                            printf("Error, can't change pull up value\n");
                            pressEnterToContinue();
                            break;
                        }
                        pullup = value;
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '7' :  printf("Get value \n");
                        ret = as_gpio_get_pullup_value(gpio_dev,
                                                       pin_num);
                        if (ret < 0)
                        {
                            printf("Error, can't get pull up value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Value is %d\n",ret);
                        pullup = ret;
                        pressEnterToContinue();
                        break;
            case '8' :  printf("1)  GPIO_IRQ_MODE_NOINT  \n");   
                        printf("2)  GPIO_IRQ_MODE_RISING \n");
                        printf("3)  GPIO_IRQ_MODE_FALLING\n");
                        printf("4)  GPIO_IRQ_MODE_BOTH   \n");
                        printf("Give value : ");
                        scanf("%d", &value);
                        if (value == 1)value = GPIO_IRQ_MODE_NOINT  ; 
                        if (value == 2)value = GPIO_IRQ_MODE_RISING ; 
                        if (value == 3)value = GPIO_IRQ_MODE_FALLING; 
                        if (value == 4)value = GPIO_IRQ_MODE_BOTH   ; 
                        ret = as_gpio_set_irq_mode(gpio_dev,
                                                   pin_num,
                                                   value);
                        if(ret < 0)
                        {
                            printf("Error, can't change irq value\n");
                            pressEnterToContinue();
                            break;
                        }
                        printf("Ok value changed\n");
                        pressEnterToContinue();
                        break;
            case '9' :  printf("Get value \n");
                        ret = as_gpio_get_irq_mode(gpio_dev, pin_num);
                        if (ret < 0)
                        {
                            printf("Error, can't get pull up value\n");
                            pressEnterToContinue();
                            break;
                        }
                        if (ret==GPIO_IRQ_MODE_NOINT  )printf("GPIO_IRQ_MODE_NOINT  \n"); 
                        if (ret==GPIO_IRQ_MODE_RISING )printf("GPIO_IRQ_MODE_RISING \n"); 
                        if (ret==GPIO_IRQ_MODE_FALLING)printf("GPIO_IRQ_MODE_FALLING\n"); 
                        if (ret==GPIO_IRQ_MODE_BOTH   )printf("GPIO_IRQ_MODE_BOTH   \n"); 
                        pressEnterToContinue();
                        break;
            case 'a' :  printf("Blocking read \n");
                        ret = as_gpio_blocking_get_pin_value(gpio_dev, pin_num);
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
}

#ifdef APF9328
#   define MAX1027_SPI_NUM (1)
#elif defined(APF27)
#   define MAX1027_SPI_NUM (0)
#else
#error Error no platform defined
#endif

#define NUM_OF_CHANNEL (8)

void test_max1027()
{
    char buffer[50];
    int ret;
    char c_value[10];
    int value;
    int averaging=1;
    int temperature = 0;
    int temp_read=0;
    struct as_max1027_device *max1027_dev;
    int channel=0;
    AS_max1027_mode mode= AS_MAX1027_SLOW;


    max1027_dev = as_max1027_open(MAX1027_SPI_NUM, mode);
    if (max1027_dev == NULL)
    {
        printf("Error, can't open max1027. Is max1027 modprobed ?\n");
        pressEnterToContinue();
        return ;
    }
    pressEnterToContinue();

    while(buffer[0] != 'q')
    {
        system("clear");
        printf("**************************\n");
        printf("   Testing max1027       *\n");
        printf("**************************\n");
        printf("Choose ('q' to quit):\n");
        printf(" 1) Change mode (%s)\n",(mode == AS_MAX1027_SLOW)?"SLOW":"FAST");
        printf(" 2) Select channel (%d)\n", channel);
        printf(" 3) Set averaging (%d)\n", averaging);
        printf(" 4) Read channel value\n");
        printf(" 5) Read temperature\n");

        printf("> ");
        scanf("%s",buffer);

        switch(buffer[0])
        {
            case '1' :  printf("Give mode wanted (s:SLOW, f:FAST): ");
                        scanf("%s",c_value);
                        if ((c_value[0]=='s') && (mode == AS_MAX1027_FAST)){
                            as_max1027_close(max1027_dev);
                            max1027_dev = as_max1027_open(MAX1027_SPI_NUM,
                                                          AS_MAX1027_SLOW);
                            if (max1027_dev == NULL){
                                printf("Error, can't open max1027 in slow mode\n");
                                pressEnterToContinue();
                                break;
                            }
                            mode = AS_MAX1027_SLOW;
                            printf("Mode changed to Slow\n");
                        } else if((c_value[0] == 'f') && (mode == AS_MAX1027_SLOW)){
                            as_max1027_close(max1027_dev);
                            max1027_dev = as_max1027_open(MAX1027_SPI_NUM,
                                                          AS_MAX1027_FAST);
                            if (max1027_dev == NULL){
                                printf("Error, can't open max1027 in fast mode\n");
                                pressEnterToContinue();
                                break;
                            }
                            mode = AS_MAX1027_FAST;
                            printf("Mode changed to Fast\n");
                        }
                        pressEnterToContinue();
                        break;
            case '2' :  printf("Give channel you want to read (0-8): ");
                        scanf("%d",&value);
                        if ((value >= NUM_OF_CHANNEL) || (value < 0)){
                            printf("Channel num wrong\n");
                            break;
                        }
                        channel = value;
                        break;
            case '3' :  printf("Give averaging (1, 4, 8, 16, 32): ");
                        scanf("%d", &value);
                        ret = as_max1027_set_averaging(max1027_dev, value);
                        if (ret < 0)
                        {
                            printf("Error, can't set averaging\n");
                            pressEnterToContinue();
                            break;
                        }
                        averaging = value;
                        pressEnterToContinue();
                        break;
            case '4' :  printf("TODO");
                        break;
            case '5' :  ret = as_max1027_read_temperature_mC(max1027_dev, &temp_read);
                        if (ret < 0) {
                            printf("Error reading temperature\n");
                            pressEnterToContinue();
                            break;
                        }
                        temperature = temp_read;
                        printf("Temperature read in mili⁰C : %d\n", temperature);
                        pressEnterToContinue();
                        break;
            default : break;
        }
    }

    ret = as_max1027_close(max1027_dev);
    if (ret < 0) {
        printf("Error on closing max1027\n");
        pressEnterToContinue();
    }
}


