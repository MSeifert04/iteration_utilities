#ifndef PYIU_DEEPFLATTEN_H
#define PYIU_DEEPFLATTEN_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iteratorlist;
    PyObject *types;
    PyObject *ignore;
    Py_ssize_t depth;
    Py_ssize_t currentdepth;
    int isstring;
} PyIUObject_DeepFlatten;

extern PyTypeObject PyIUType_DeepFlatten;

#endif
