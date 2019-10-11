#ifndef PYIU_NTH_H
#define PYIU_NTH_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    Py_ssize_t index;
} PyIUObject_Nth;

extern PyTypeObject PyIUType_Nth;

PyObject * PyIUNth_New(Py_ssize_t index);

#endif
