#ifndef PYIU_FLIP_H
#define PYIU_FLIP_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
#if PyIU_USE_VECTORCALL
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Flip;

extern PyTypeObject PyIUType_Flip;

#ifdef __cplusplus
}
#endif

#endif
