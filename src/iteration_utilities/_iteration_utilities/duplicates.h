#ifndef PYIU_DUPLICATES_H
#define PYIU_DUPLICATES_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *key;
    PyObject *seen;
} PyIUObject_Duplicates;

extern PyTypeObject PyIUType_Duplicates;

#ifdef __cplusplus
}
#endif

#endif
