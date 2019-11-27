#ifndef PYIU_STARFILTER_H
#define PYIU_STARFILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *iterator;
} PyIUObject_Starfilter;

extern PyTypeObject PyIUType_Starfilter;

#ifdef __cplusplus
}
#endif

#endif
