#ifndef PYIU_CONSTANT_H
#define PYIU_CONSTANT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *item;
#if PyIU_USE_VECTORCALL
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Constant;

extern PyTypeObject PyIUType_Constant;

PyObject * PyIUConstant_New(PyObject *value);

#ifdef __cplusplus
}
#endif

#endif
