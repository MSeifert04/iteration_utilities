#ifndef PYIU_COMPLEMENT_H
#define PYIU_COMPLEMENT_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Complement;

extern PyTypeObject PyIUType_Complement;

#endif
