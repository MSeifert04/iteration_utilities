#ifndef PYIU_GROUPER_H
#define PYIU_GROUPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *fillvalue;
    PyObject *result;
    Py_ssize_t times;
    int truncate;
} PyIUObject_Grouper;

extern PyTypeObject PyIUType_Grouper;

#ifdef __cplusplus
}
#endif

#endif
