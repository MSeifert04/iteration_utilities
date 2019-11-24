#ifndef PYIU_TABULATE_H
#define PYIU_TABULATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *cnt;
} PyIUObject_Tabulate;

extern PyTypeObject PyIUType_Tabulate;

#ifdef __cplusplus
}
#endif

#endif
