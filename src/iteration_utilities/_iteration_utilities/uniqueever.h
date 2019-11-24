#ifndef PYIU_UNIQUEEVER_H
#define PYIU_UNIQUEEVER_H

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
} PyIUObject_UniqueEver;

extern PyTypeObject PyIUType_UniqueEver;

#ifdef __cplusplus
}
#endif

#endif
