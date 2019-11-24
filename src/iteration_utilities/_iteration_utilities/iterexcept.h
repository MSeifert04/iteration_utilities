#ifndef PYIU_ITEREXCEPT_H
#define PYIU_ITEREXCEPT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *except;
    PyObject *first;
} PyIUObject_Iterexcept;

extern PyTypeObject PyIUType_Iterexcept;

#ifdef __cplusplus
}
#endif

#endif
