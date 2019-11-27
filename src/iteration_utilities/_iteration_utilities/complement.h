#ifndef PYIU_COMPLEMENT_H
#define PYIU_COMPLEMENT_H

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
} PyIUObject_Complement;

extern PyTypeObject PyIUType_Complement;

#ifdef __cplusplus
}
#endif

#endif
