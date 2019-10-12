#ifndef PYIU_PARTIAL_H
#define PYIU_PARTIAL_H

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
} PyIUObject_Partial;

extern PyTypeObject PyIUType_Partial;

#endif
