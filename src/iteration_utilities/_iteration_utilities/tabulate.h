#ifndef PYIU_TABULATE_H
#define PYIU_TABULATE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *cnt;
} PyIUObject_Tabulate;

extern PyTypeObject PyIUType_Tabulate;

#endif
