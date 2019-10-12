#ifndef PYIU_CHAINED_H
#define PYIU_CHAINED_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *funcs;
    int all;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Chained;

extern PyTypeObject PyIUType_Chained;

#endif
