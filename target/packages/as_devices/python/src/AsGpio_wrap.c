/*
**    THE ARMadeus Systems
** 
**    Copyright (C) 2010  The armadeus systems team 
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
#include "AsGpio_wrap.h"
#include "as_gpio.h"

/* Init module */
//void initAsGpio();

/** @brief Initialize port access
 * @param aPortChar character port in UPPER case
 * @return PyObject 
 */
static PyObject * gpio_open(PyObject *self, PyObject *args)
{
    /* parameters */
    char aPortChar;
    struct as_gpio_device *dev;
    PyObject *ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "c", &aPortChar))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    
    dev = as_gpio_open(aPortChar);
    if (dev == NULL)
    {
        PyErr_SetString(PyExc_IOError,
                        "Initialization error. Is kernel module loaded ?");
        return NULL;
    }
    ret = Py_BuildValue("(c,i)",
                        dev->port_letter,
                        dev->fdev);
    free(dev);
    return ret;
}

/** @brief  Set pin direction
 *
 * @param (c,i) -> (aPort_letter, aFdev) as_gpio_device structure_list
 * @param aPinNum pin number
 * @param aDirection direction 0:input 1:output
 *
 * @return error if negative value 
 */
static PyObject * setPinDirection(PyObject *self, PyObject *args)
{
    /* parameters */
    char aPort_letter;
    int aFdev;

    int aPinNum;
    int aDirection;

    struct as_gpio_device *dev;
    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "(c,i)ii", 
                          &aPort_letter, 
                          &aFdev,
                          &aPinNum,
                          &aDirection))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
   
    dev = (struct as_gpio_device *)malloc(sizeof(struct as_gpio_device));
    if (dev == NULL)
    {
        PyErr_SetString(PyExc_IOError,
                        "memory allocation: can't malloc fdev structure");
        return NULL;
    }
    dev->port_letter = aPort_letter;
    dev->fdev = aFdev;

    ret = as_gpio_set_pin_direction(dev, aPinNum, aDirection);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't set pin direction");
        return NULL;
    }

    free(dev);
    return Py_BuildValue("i", ret);
}

static PyObject * setPinValue(PyObject *self, PyObject *args)
{
    return NULL;
}
static PyObject * getPinValue(PyObject *self, PyObject *args)
{
    return NULL;
}

static PyObject * gpio_close(PyObject *self, PyObject *args)
{
    return NULL;
}


