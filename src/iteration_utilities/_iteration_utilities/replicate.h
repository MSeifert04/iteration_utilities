#ifndef PYIU_REPLICATE_H
#define PYIU_REPLICATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *current;
    Py_ssize_t repeattotal;
    Py_ssize_t repeatcurrent;
} PyIUObject_Replicate;

extern PyTypeObject PyIUType_Replicate;

#ifdef __cplusplus
}
#endif

#endif
