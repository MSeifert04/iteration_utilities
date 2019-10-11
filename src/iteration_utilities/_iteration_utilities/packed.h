#ifndef PYIU_PACKED_H
#define PYIU_PACKED_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Packed;

extern PyTypeObject PyIUType_Packed;

#endif
