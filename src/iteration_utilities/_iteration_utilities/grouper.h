#ifndef PYIU_GROUPER_H
#define PYIU_GROUPER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *fillvalue;
    Py_ssize_t times;
    int truncate;
} PyIUObject_Grouper;

extern PyTypeObject PyIUType_Grouper;

#endif
