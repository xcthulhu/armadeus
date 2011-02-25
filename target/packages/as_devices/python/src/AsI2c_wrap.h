/*
**    THE ARMadeus Systems
**
**    Copyright (C) 2011  The armadeus systems team
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


#include <Python.h>

/* Init module */
void initAsI2c_wrap();

/*********************/
/* Functions wrapped */
/*********************/
static PyObject * i2c_open(PyObject *self, PyObject *args);
static PyObject * i2c_close(PyObject *self, PyObject *args);

static PyObject * i2c_set_slave_addr(PyObject *self, PyObject *args);
static PyObject * i2c_get_slave_addr(PyObject *self, PyObject *args);

static PyObject * i2c_read(PyObject *self, PyObject *args);
static PyObject * i2c_write(PyObject *self, PyObject *args);

static PyObject * i2c_read_reg(PyObject *self, PyObject *args);
static PyObject * i2c_write_reg(PyObject *self, PyObject *args);

static PyObject * i2c_read_reg_byte(PyObject *self, PyObject *args);
static PyObject * i2c_write_reg_byte(PyObject *self, PyObject *args);

static PyObject * i2c_read_msg(PyObject *self, PyObject *args);
