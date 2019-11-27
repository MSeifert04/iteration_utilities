#ifndef PYIU_ACCUMULATE_H
#define PYIU_ACCUMULATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *binop;
    PyObject *total;
} PyIUObject_Accumulate;

extern PyTypeObject PyIUType_Accumulate;

#ifdef __cplusplus
}
#endif

#endif
