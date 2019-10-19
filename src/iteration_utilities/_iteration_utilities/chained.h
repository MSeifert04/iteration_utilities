#ifndef PYIU_CHAINED_H
#define PYIU_CHAINED_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *funcs;
    int all;
#if PyIU_USE_VECTORCALL
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Chained;

extern PyTypeObject PyIUType_Chained;

#endif
