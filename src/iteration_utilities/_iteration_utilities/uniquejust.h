#ifndef PYIU_UNIQUEJUST_H
#define PYIU_UNIQUEJUST_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *keyfunc;
    PyObject *lastitem;
} PyIUObject_UniqueJust;

extern PyTypeObject PyIUType_UniqueJust;

#endif
