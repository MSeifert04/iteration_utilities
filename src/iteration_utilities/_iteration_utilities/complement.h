#ifndef PYIU_COMPLEMENT_H
#define PYIU_COMPLEMENT_H

#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *func;
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 8
    vectorcallfunc vectorcall;
#endif
} PyIUObject_Complement;

extern PyTypeObject PyIUType_Complement;

#endif
