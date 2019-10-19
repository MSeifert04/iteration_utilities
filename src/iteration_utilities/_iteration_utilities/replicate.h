#ifndef PYIU_REPLICATE_H
#define PYIU_REPLICATE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *current;
    Py_ssize_t repeattotal;
    Py_ssize_t repeatcurrent;
} PyIUObject_Replicate;

extern PyTypeObject PyIUType_Replicate;

#endif
