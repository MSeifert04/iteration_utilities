#ifndef PYIU_INTERSPERSE_H
#define PYIU_INTERSPERSE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *filler;
    PyObject *nextitem;
    int started;
} PyIUObject_Intersperse;

extern PyTypeObject PyIUType_Intersperse;

#endif
