/*
 **    C wrapper for GPIOs usage.
 **
 **    Copyright (C) 2009, 2010, 2011  The Armadeus Project - ARMadeus Systems
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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */
#include <sys/select.h>
#include <poll.h>
#include <errno.h>

#include <sys/ioctl.h>

#include "as_helpers.h"
#include "as_gpio.h"

#define BUFF_SIZE 300

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

/*------------------------------------------------------------------------------*/

struct as_gpio_device *as_gpio_open(char aPortChar, int aPinNum)
{
    struct as_gpio_device *dev;
    int pin_file;
    int export_file;
    int retval;
    int port_num;
    char buf[BUFF_SIZE];

    if (((aPortChar - 'A') > 5) || ((aPortChar - 'A') < 0) ) {
        ERROR("Wrong port name %c\n", aPortChar);
        return NULL;
    }
    if ((aPinNum < 0) || (aPinNum >= PORT_SIZE)) {
        ERROR("Wrong port num %d\n", aPinNum);
        return NULL;
    }
    port_num = (aPortChar-'A')*PORT_SIZE + aPinNum;

    export_file = open("/sys/class/gpio/export", O_WRONLY);
    if (export_file < 0) {
        ERROR("Can't open /sys/class/gpio/export\nBe sure that gpiolib is under your kernel\n");
        return NULL;
    }

    retval = as_write_buffer(export_file, port_num);
    if (retval < 0) {
        ERROR("/sys/class/gpio/export can't be written\n");
        close(export_file);
        return NULL;
    }
    close(export_file);

    sleep(1); //XXX

    snprintf(buf, BUFF_SIZE, "/sys/class/gpio/gpio%d/value", port_num);
    pin_file = open(buf, O_RDWR);
    if (pin_file < 0) {
        ERROR("Can't export gpio number %d (port %c pin %d)\n", port_num, aPortChar, aPinNum);
        return NULL;
    }
    close(pin_file);

    dev = malloc(sizeof(struct as_gpio_device));
    if (dev == NULL) {
        ERROR("Can't allocate as_gpio_device structure\n");
        return NULL;
    }

    dev->port_num = port_num;
    dev->port_letter = aPortChar;
    dev->pin_number = aPinNum;

    return dev;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_direction(struct as_gpio_device *gpio_dev, char *direction)
{
	char buf[BUFF_SIZE];
	int gpio_fd;
	int ret = 0;

	snprintf(buf, BUFF_SIZE, "/sys/class/gpio/gpio%d/direction", gpio_dev->port_num);
	gpio_fd = open(buf, O_WRONLY);
	if (gpio_fd < 0) {
		ERROR("Can't open gpio%d direction\n", gpio_dev->port_num);
		return -EINVAL;
	}

	ret = as_write_buffer_string(gpio_fd, direction);
	if (ret < 0)
		ERROR("Error writing direction\n");

	close(gpio_fd);

	return ret;
}

/*------------------------------------------------------------------------------*/

const char* as_gpio_get_pin_direction(struct as_gpio_device *gpio_dev)
{
	char buf[BUFF_SIZE];
	static char direction[4];
	FILE *gpio_file;
	int ret = 0;
	static const char none[] = "??";

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction",
			gpio_dev->port_num);
	gpio_file = fopen(buf, "r");
	if (!gpio_file) {
		ERROR("Can't open gpio%d direction\n", gpio_dev->port_num);
		return none;
	}
	ret = fscanf(gpio_file, "%s", direction);
	fclose(gpio_file);
	if (ret < 0) {
		ERROR("Can't get gpio%d direction\n", gpio_dev->port_num);
		return none;
	}

	return direction;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_pin_value(struct as_gpio_device *aDev, int aValue)
{
    char buf[BUFF_SIZE];
    int pin_file;
    int retval;

    snprintf(buf, BUFF_SIZE, "/sys/class/gpio/gpio%d/value", aDev->port_num);
    pin_file = open(buf, O_WRONLY);
    if (pin_file < 0) {
        ERROR("Can't open gpio%d direction\n", aDev->port_num);
        return -1;
    }

    retval = as_write_buffer(pin_file, aValue);
    if (retval < 0) {
        ERROR("Can't write value\n");
        close(pin_file);
        return -1;
    }
    close(pin_file);

    return aValue;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_value(struct as_gpio_device *aDev)
{
    char buf[BUFF_SIZE];
    int pin_file;
    int retval;
    int value;

    snprintf(buf, BUFF_SIZE, "/sys/class/gpio/gpio%d/value", aDev->port_num);
    pin_file = open(buf, O_RDONLY);
    if (pin_file < 0) {
        ERROR("Can't open gpio%d value\n", aDev->port_num);
        return -1;
    }

    retval = as_read_int(pin_file, &value);
    if (retval < 0) {
        ERROR("Can't read value\n");
        close(pin_file);
        return -1;
    }
    close(pin_file);

    return value;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_wait_event(struct as_gpio_device *gpio_dev, int delay_ms)
{
	char gpio_sys_name[BUFF_SIZE];
	int gpio_fd;
	struct pollfd poll_set[1];
	int ret;
	char tmp[16];

	snprintf(gpio_sys_name, BUFF_SIZE, "/sys/class/gpio/gpio%d/value",
				gpio_dev->port_num);
	gpio_fd = open(gpio_sys_name, O_RDONLY);
	if (gpio_fd < 0) {
        	ERROR("Can't open gpio%d value\n", gpio_dev->port_num);
		return -EINVAL;
	}

	/* flush pending event */
	read(gpio_fd, tmp, sizeof(tmp));

	poll_set[0].fd = gpio_fd;
	poll_set[0].events = POLLPRI;
	poll_set[0].revents = 0;
	ret = poll(poll_set, 1, delay_ms);
	if (ret > 0) {
		printf("event received\n");
		read(gpio_fd, tmp, sizeof(tmp));
	}

	return 0;
}

/*------------------------------------------------------------------------------*/

const char* as_gpio_get_irq_mode(struct as_gpio_device *gpio_dev)
{
	char buf[BUFF_SIZE];
	static char mode[8];
	FILE *gpio_file;
	int ret = 0;
	static const char unknown[] = "??";

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", gpio_dev->port_num);
	gpio_file = fopen(buf, "r");
	if (!gpio_file) {
		ERROR("Can't open gpio%d edge\n", gpio_dev->port_num);
		return unknown;
	}
	ret = fscanf(gpio_file, "%s", mode);
	fclose(gpio_file);
	if (ret < 0) {
		ERROR("Can't get gpio%d edge\n", gpio_dev->port_num);
		return unknown;
	}

	return mode;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_set_irq_mode(struct as_gpio_device *gpio_dev, char *mode)
{
	char buf[BUFF_SIZE];
	int gpio_fd;
	int ret = 0;

	snprintf(buf, BUFF_SIZE, "/sys/class/gpio/gpio%d/edge", gpio_dev->port_num);
	gpio_fd = open(buf, O_WRONLY);
	if (gpio_fd < 0) {
		ERROR("Can't open gpio%d edge\n", gpio_dev->port_num);
		return -EINVAL;
	}

	ret = as_write_buffer_string(gpio_fd, mode);
	if (ret < 0)
		ERROR("Error writing mode\n");

	close(gpio_fd);

	return ret;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_get_pin_num(struct as_gpio_device *aDev)
{
    return aDev->pin_number;
}

/*------------------------------------------------------------------------------*/

char as_gpio_get_port_letter(struct as_gpio_device *aDev)
{
    return aDev->port_letter;
}

/*------------------------------------------------------------------------------*/

int32_t as_gpio_close(struct as_gpio_device *aDev)
{
    return 0;
}

