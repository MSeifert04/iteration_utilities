#ifndef PYIU_ONE_H
#define PYIU_ONE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_One(PyObject *Py_UNUSED(m), PyObject *iterable);

#endif
