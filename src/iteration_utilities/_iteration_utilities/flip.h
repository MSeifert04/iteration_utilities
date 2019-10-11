#ifndef PYIU_FLIP_H
#define PYIU_FLIP_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Flip;

extern PyTypeObject PyIUType_Flip;

#endif
