#ifndef PYIU_ALLDISTINCT_H
#define PYIU_ALLDISTINCT_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_AllDistinct(PyObject *Py_UNUSED(m), PyObject *iterable);

#endif
