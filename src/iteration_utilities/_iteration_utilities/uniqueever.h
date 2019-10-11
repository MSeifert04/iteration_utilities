#ifndef PYIU_UNIQUEEVER_H
#define PYIU_UNIQUEEVER_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *key;
    PyObject *seen;
} PyIUObject_UniqueEver;

extern PyTypeObject PyIUType_UniqueEver;

#endif
