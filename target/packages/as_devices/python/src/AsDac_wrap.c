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
#include "AsDac_wrap.h"
#include "as_dac.h"

/* Methods definitions */
static PyMethodDef AsDac_wrap_methods[] = {
    {"dac_open", dac_open, METH_VARARGS, "Initialize dac"},
    {"dac_setValueInMillivolts", dac_setValueInMillivolts, METH_VARARGS, "Set Channel value in millivolts"},
    {"dac_close", dac_close, METH_VARARGS, "Close dac"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

/* Init module */
void initAsDac_wrap() /* called on first import */
{                       /* name matter if called dynamically */
    (void) Py_InitModule("AsDac_wrap", AsDac_wrap_methods); /* mod name, table ptr */
}

/** @brief Initialize pin port access
 * @return PyObject
 */
static PyObject * dac_open(PyObject *self, PyObject *args)
{
    /* parameters */
    char *aDacType;
    int aBusNumber;
    int aAddress;
    int aVref;

    struct as_dac_device *dev;
    PyObject *ret;
    char buff[300];
    /* Get arguments */
    if (!PyArg_ParseTuple(args, "siii", &aDacType,
                          &aBusNumber, &aAddress, &aVref))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    dev = as_dac_open(aDacType, aBusNumber, aAddress, aVref);
    if (dev == NULL)
    {
        snprintf(buff, 300,
                "Initialization error for DAC type %s bus %d. Is kernel module loaded ?",
                aDacType, aBusNumber);
        PyErr_SetString(PyExc_IOError,buff);
        return NULL;
    }

    ret = Py_BuildValue("l", (long)dev);

    return ret;
}

static PyObject * dac_setValueInMillivolts(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_dac_device *aDev;
    int aChannel;
    int aValue;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "lii", (long *)&aDev, &aChannel, &aValue))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_dac_set_value_in_millivolts(aDev, aChannel, aValue);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't get pin direction");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * dac_close(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_dac_device *aDev;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "l", (long *)&aDev))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_dac_close(aDev);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't close port");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}


