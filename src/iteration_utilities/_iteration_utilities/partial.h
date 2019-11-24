#ifndef PYIU_PARTIAL_H
#define PYIU_PARTIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *fn;
    PyObject *args;
    PyObject *kw;
    PyObject *dict;
    PyObject *weakreflist; /* List of weak references */
    Py_ssize_t numph;
    Py_ssize_t *posph;
#if PyIU_USE_VECTORCALL
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Partial;

extern PyTypeObject PyIUType_Partial;

#ifdef __cplusplus
}
#endif

#endif
