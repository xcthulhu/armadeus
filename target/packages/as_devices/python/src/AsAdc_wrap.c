/*
**    The ARMadeus Project
**
**    Copyright (C) 2011 The armadeus systems team
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
#include "AsAdc_wrap.h"
#include "as_adc.h"


/* Methods definitions */
static PyMethodDef AsAdc_wrap_methods[] = {
    {"adc_open", adc_open, METH_VARARGS, "Initialize adc"},
    {"getValueInMillivolts", getValueInMillivolts, METH_VARARGS, "Get Channel value in millivolts"},
    {"adc_close", adc_close, METH_VARARGS, "Close adc"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/* Init module */
void initAsAdc_wrap() /* called on first import */
{                       /* name matter if called dynamically */
    (void) Py_InitModule("AsAdc_wrap", AsAdc_wrap_methods); /* mod name, table ptr */
}

/** @brief Initialize pin port access
 * @return PyObject
 */
static PyObject * adc_open(PyObject *self, PyObject *args)
{
    /* parameters */
    char *aAdcType;
    int aDeviceNum;
    int aVref;

    struct as_adc_device *dev;
    PyObject *ret;
    char buff[300];
    /* Get arguments */
    if (!PyArg_ParseTuple(args, "sii", &aAdcType, &aDeviceNum, &aVref))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    dev = as_adc_open(aAdcType, aDeviceNum, aVref);
    if (dev == NULL)
    {
        snprintf(buff, 300,
                "Initialization error for ADC type %s bus %d. Is kernel module loaded ?",
                aAdcType, aDeviceNum);
        PyErr_SetString(PyExc_IOError,buff);
        return NULL;
    }

    ret = Py_BuildValue("l", (long)dev);

    return ret;
}

static PyObject * getValueInMillivolts(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_adc_device *aFdev;
    int aChannel;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "li", (long *)&aFdev, &aChannel))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_adc_get_value_in_millivolts(aFdev, aChannel);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't get pin direction");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * adc_close(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_adc_device *aFdev;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "l", (long *)&aFdev))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_adc_close(aFdev);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't close port");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}


