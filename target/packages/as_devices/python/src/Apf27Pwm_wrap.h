#include <Python.h>


/* init module */
void initApf27Pwm();

/*********************/
/* Functions wrapped */
/*********************/
static PyObject * init(PyObject *self, PyObject *args);

static PyObject * setFrequency(PyObject *self, PyObject *args);
static PyObject * getFrequency(PyObject *self, PyObject *args);

static PyObject * setPeriod(PyObject *self, PyObject *args);
static PyObject * getPeriod(PyObject *self, PyObject *args);

static PyObject * setDuty(PyObject *self, PyObject *args);
static PyObject * getDuty(PyObject *self, PyObject *args);

static PyObject * activate(PyObject *self, PyObject *args);
static PyObject * getState(PyObject *self, PyObject *args);

static PyObject * pwm_close(PyObject *self, PyObject *args);

/* Methods definition */
static PyMethodDef Apf27Pwm_wrap_Methods[] = {
    {"init", init  , METH_VARARGS, "Initialize pwm"},
    {"setFrequency", setFrequency , METH_VARARGS, "Set frequency in Hz"},
    {"getFrequency", getFrequency , METH_VARARGS, "Get frequency in Hz"},
    {"setPeriod"   , setPeriod    , METH_VARARGS, "Set period in us"},
    {"getPeriod"   , getPeriod    , METH_VARARGS, "Get period in us"},
    {"setDuty"     , setDuty      , METH_VARARGS, "Set duty in 1/1000"},
    {"getDuty"     , getDuty      , METH_VARARGS, "Get duty in 1/1000"},
    {"activate"    , activate     , METH_VARARGS, "Activate or desactivate pwm"},
    {"getState"    , getState     , METH_VARARGS, "Get state of pwm (activated or not"},
    {"pwm_close"   , pwm_close    , METH_VARARGS, "Close pwm"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


