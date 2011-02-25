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
*
* TODO:
* - Suppress MAX_DATA_SIZE macro
*/
#include "AsI2c_wrap.h"
#include "as_i2c.h"

#define MAX_DATA_SIZE 20

/* Methods definitions */
static PyMethodDef AsI2c_wrap_methods[] = {
    {"i2c_open"          , i2c_open          , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_close"         , i2c_close         , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_set_slave_addr", i2c_set_slave_addr, METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_get_slave_addr", i2c_get_slave_addr, METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_read"          , i2c_read          , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_write"         , i2c_write         , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_read_reg"      , i2c_read_reg      , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_write_reg"     , i2c_write_reg     , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_read_reg_byte" , i2c_read_reg_byte , METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_write_reg_byte", i2c_write_reg_byte, METH_VARARGS, "as_i2c function wrapped"},
    {"i2c_read_msg"      , i2c_read_msg , METH_VARARGS, "as_i2c function wrapped"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};


/* Init module */
void initAsI2c_wrap() /* called on first import */
{                       /* name matter if called dynamically */
    (void) Py_InitModule("AsI2c_wrap", AsI2c_wrap_methods); /* mod name, table ptr */
}

static PyObject * i2c_open(PyObject *self, PyObject *args)
{
    /* parameters */
    int aBusNumber;

    struct as_i2c_device *dev;
    PyObject *ret;
    char buff[300];
    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aBusNumber))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    dev = as_i2c_open(aBusNumber);
    if (dev == NULL)
    {
        snprintf(buff, 300,
                "Initialization error for i2c bus %d.", aBusNumber);
        PyErr_SetString(PyExc_IOError,buff);
        return NULL;
    }
    ret = Py_BuildValue("l", (long)dev);

    return ret;
}

static PyObject * i2c_close(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;

    /* return value */
    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "l", (long *)&aDev)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    ret = as_i2c_close(aDev);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError, "Can't close i2c");
        return Py_BuildValue("i", -1);
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_set_slave_addr(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    int aSlaveAddr;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "li", (long *)&aDev, &aSlaveAddr))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_set_slave_addr(aDev, aSlaveAddr);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't set slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_get_slave_addr(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "l", (long *)&aDev))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_get_slave_addr(aDev);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_read(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t aData[MAX_DATA_SIZE];
    int aSize;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "li", (long *)&aDev, &aSize))
    {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    if (aSize > MAX_DATA_SIZE) {
        PyErr_SetString(PyExc_IOError,
                        "Size can't be upper than "+str(MAX_DATA_SIZE));
        return NULL;
    }

    ret = as_i2c_read(aDev, aData, aSize);
    if (ret < 0)
    {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("s#", (unsigned char *)aData, aSize);
}

static PyObject * i2c_write(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t *aData;
    int aSize;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "ls#", (long *)&aDev, (unsigned char *)&aData, &aSize)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_write(aDev, aData, aSize);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_read_reg(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t aReg;
    uint8_t aData[MAX_DATA_SIZE];
    int aSize;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "lii", (long *)&aDev, &aReg, &aSize)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    if (aSize > MAX_DATA_SIZE) {
        PyErr_SetString(PyExc_IOError,
                        "Size can't be upper than "+str(MAX_DATA_SIZE));
        return NULL;
    }

    ret = as_i2c_read_reg(aDev, aReg, aData, aSize);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("s#", (unsigned char *)aData, aSize);
}

static PyObject * i2c_write_reg(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t aReg;
    uint8_t *aData;
    int aSize;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "lis#", (long *)&aDev, &aReg, (unsigned char *)&aData, &aSize)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_write_reg(aDev, aReg, aData, aSize);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_read_reg_byte(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t aReg;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "li", (long *)&aDev, &aReg)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_read_reg_byte(aDev, aReg);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

static PyObject * i2c_read_msg(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t *aWData;
    int aWriteSize;
    uint8_t RData[MAX_DATA_SIZE];
    int aReadSize;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "ls#i", (long *)&aDev,
                                        (unsigned char *)&aWData, &aWriteSize,
                                        &aReadSize)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }
    if (aReadSize > MAX_DATA_SIZE) {
        PyErr_SetString(PyExc_IOError,
                        "Read size can't be upper than "+str(MAX_DATA_SIZE));
        return NULL;
    }

    ret = as_i2c_read_msg(aDev, aWData, aWriteSize, RData, aReadSize);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't forge read message");
        return NULL;
    }

    return Py_BuildValue("s#", (unsigned char *)RData, aReadSize);
}


static PyObject * i2c_write_reg_byte(PyObject *self, PyObject *args)
{
    /* parameters */
    struct as_i2c_device *aDev;
    uint8_t aReg;
    uint8_t aVal;

    int ret;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "lii", (long *)&aDev, &aReg, &aVal)) {
        PyErr_SetString(PyExc_IOError,
                        "Wrong parameters.");
        return NULL;
    }

    ret = as_i2c_write_reg_byte(aDev, aReg, aVal);
    if (ret < 0) {
        PyErr_SetString(PyExc_IOError,
                        "Can't get slave address");
        return NULL;
    }

    return Py_BuildValue("i", ret);
}

