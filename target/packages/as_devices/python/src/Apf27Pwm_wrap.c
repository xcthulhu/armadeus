#include "Apf27Pwm_wrap.h"
#include "as_apf27_pwm.h"

/** Initialize pwm
 * @param aPwmNumber The pwm number used
 * @return PyObject
 */
static PyObject * init(PyObject *self, PyObject *args)
{
    int aPwmNumber;
    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_init(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_IOError, 
                        "Initialization error. Is kernel module loaded ?");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

/** Set frequency
 * @param aPwmNumber the pwm number used
 * @param aFrequency frequency to set in Hz
 */
static PyObject * setFrequency(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;
    int aFrequency;

    int ret=0;

    /* Get arguments */
    if(!PyArg_ParseTuple(args, "ii", &aPwmNumber, &aFrequency))
    {
        return NULL;
    }
    
    ret = as_apf27_pwm_setFrequency(aPwmNumber, aFrequency);
    if(ret<0)
    {
        PyErr_SetString(PyExc_Exception,"Can't set frequency");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

/** Get Frequency
 * @param aPwmNumber the pwm number used
 */
static PyObject * getFrequency(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_getFrequency(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't get frequency");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

static PyObject * setPeriod(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;
    int aPeriod;

    int ret=0;

    /* Get arguments */
    if(!PyArg_ParseTuple(args, "ii", &aPwmNumber, &aPeriod))
    {
        return NULL;
    }
    
    ret = as_apf27_pwm_setPeriod(aPwmNumber, aPeriod);
    if(ret<0)
    {
        PyErr_SetString(PyExc_Exception,"Can't set period");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

static PyObject * getPeriod(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_getPeriod(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't get period");
        return NULL;
    }

    return Py_BuildValue("i",ret);

}

static PyObject * setDuty(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;
    int aDuty;

    int ret=0;

    /* Get arguments */
    if(!PyArg_ParseTuple(args, "ii", &aPwmNumber, &aDuty))
    {
        return NULL;
    }
    
    ret = as_apf27_pwm_setDuty(aPwmNumber, aDuty);
    if(ret<0)
    {
        PyErr_SetString(PyExc_Exception,"Can't set duty");
        return NULL;
    }

    return Py_BuildValue("i",ret);

}

static PyObject * getDuty(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_getDuty(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't get duty");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

static PyObject * activate(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;
    int aEnable;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "ii", &aPwmNumber,&aEnable))
        return NULL;

    ret = as_apf27_pwm_activate(aPwmNumber,aEnable);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't activate pwm");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

static PyObject * getState(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_getState(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't get state");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

static PyObject * pwm_close(PyObject *self, PyObject *args)
{
    /* parameters */
    int aPwmNumber;

    int ret=1;

    /* Get arguments */
    if (!PyArg_ParseTuple(args, "i", &aPwmNumber))
        return NULL;

    ret = as_apf27_pwm_close(aPwmNumber);
    if(ret < 0)
    {
        PyErr_SetString(PyExc_Exception, 
                        "Can't close");
        return NULL;
    }

    return Py_BuildValue("i",ret);
}

void initApf27Pwm_wrap()
{
    PyObject *m;

    m = Py_InitModule("Apf27Pwm_wrap", Apf27Pwm_wrap_Methods);
    if (m == NULL)
        return;

}

