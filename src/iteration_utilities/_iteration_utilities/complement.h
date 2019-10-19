#ifndef PYIU_COMPLEMENT_H
#define PYIU_COMPLEMENT_H

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

#endif
