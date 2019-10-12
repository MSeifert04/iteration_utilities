#ifndef PYIU_CONSTANT_H
#define PYIU_CONSTANT_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *item;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Constant;

extern PyTypeObject PyIUType_Constant;

PyObject * PyIUConstant_New(PyObject *value);

#endif
