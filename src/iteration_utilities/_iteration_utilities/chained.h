#ifndef PYIU_CHAINED_H
#define PYIU_CHAINED_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *funcs;
    int all;
} PyIUObject_Chained;

extern PyTypeObject PyIUType_Chained;

#endif
