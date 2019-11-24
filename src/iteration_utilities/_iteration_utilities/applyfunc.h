#ifndef PYIU_APPLYFUNC_H
#define PYIU_APPLYFUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *value;
} PyIUObject_Applyfunc;

extern PyTypeObject PyIUType_Applyfunc;

#ifdef __cplusplus
}
#endif

#endif
