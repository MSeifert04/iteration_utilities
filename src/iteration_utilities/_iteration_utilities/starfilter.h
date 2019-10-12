#ifndef PYIU_STARFILTER_H
#define PYIU_STARFILTER_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *iterator;
} PyIUObject_Starfilter;

extern PyTypeObject PyIUType_Starfilter;

#endif
