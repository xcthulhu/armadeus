#include "as_as1531.h"
#include "as_helpers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> /* for close() */

#define AS_AS1531_MAX_VALUE 2500
#define BUFF_SIZE (300)

#undef ERROR
#define ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)

struct as_adc_device *as_adc_open_as1531(int aDeviceNum, int aVRef) {
    struct as_adc_device *dev;
    int ret;
    int fname;
    char buf[BUFF_SIZE];

    snprintf(buf, BUFF_SIZE, "/sys/class/hwmon/hwmon%d/device/name", aDeviceNum);
    fname = open(buf, O_RDONLY);
    if (fname < 0) {
        ERROR("Can't open file %s, is as1531 modprobed ?\n", buf);
        return NULL;
    }

    ret = read(fname, buf, BUFF_SIZE);
    if (ret <= 0) {
        ERROR("Can't read name\n");
        return NULL;
    }

    ret = strncmp(AS_AS1531_NAME, buf, strlen(AS_AS1531_NAME));
    if (ret != 0) {
        ERROR("Wrong driver name %s\n",buf);
        return NULL;
    }

    close(fname);

    dev = malloc(sizeof(struct as_adc_device));
    if (dev == NULL) {
        ERROR("Can't allocate memory for as_adc structure\n");
        return NULL;
    }

    dev->device_type = "as1531";
    dev->device_num  = aDeviceNum;
    dev->vref = aVRef;
    return dev;
}

int32_t as_adc_get_value_in_millivolts_as1531(struct as_adc_device *aDev, int aChannel) {
    int ret;
    int finput;
    int value;
    char buf[BUFF_SIZE];

    snprintf(buf, BUFF_SIZE,
             "/sys/class/hwmon/hwmon%d/device/in%d_input",
             aDev->device_num, aChannel);
    finput = open(buf, O_RDONLY);
    if (finput < 0) {
        ERROR("Can't open file %s.\n", buf);
        return -1;
    }

    ret = as_read_int(finput, &value);
    if (ret < 0) {
        ERROR("Can't read value on channel %d\n", aChannel);
        return -1;
    }
    close(finput);

    if (AS_AS1531_MAX_VALUE == aDev->vref) return value;

    return (value*aDev->vref)/AS_AS1531_MAX_VALUE;
}

int32_t as_adc_close_as1531(struct as_adc_device *aDev) {
    return 0;
}
