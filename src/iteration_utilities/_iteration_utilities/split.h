#ifndef PYIU_SPLIT_H
#define PYIU_SPLIT_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    int keep_delimiter;
    int keep_before;
    int keep_after;
    int cmp;
    PyObject *next;
} PyIUObject_Split;

extern PyTypeObject PyIUType_Split;

#endif
