#ifndef PYIU_ROUNDROBIN_H
#define PYIU_ROUNDROBIN_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    Py_ssize_t numactive;
    Py_ssize_t active;
} PyIUObject_Roundrobin;

extern PyTypeObject PyIUType_Roundrobin;

#endif
