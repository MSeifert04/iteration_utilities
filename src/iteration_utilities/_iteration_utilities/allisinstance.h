#ifndef PYIU_ALLISINSTANCE_H
#define PYIU_ALLISINSTANCE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_AllIsinstance(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#endif
