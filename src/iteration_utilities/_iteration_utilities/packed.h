#ifndef PYIU_PACKED_H
#define PYIU_PACKED_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Packed;

extern PyTypeObject PyIUType_Packed;

#endif
