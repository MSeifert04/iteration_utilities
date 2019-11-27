#ifndef PYIU_UNIQUEJUST_H
#define PYIU_UNIQUEJUST_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *keyfunc;
    PyObject *lastitem;
} PyIUObject_UniqueJust;

extern PyTypeObject PyIUType_UniqueJust;

#ifdef __cplusplus
}
#endif

#endif
