#ifndef PYIU_FLIP_H
#define PYIU_FLIP_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Flip;

extern PyTypeObject PyIUType_Flip;

#endif
