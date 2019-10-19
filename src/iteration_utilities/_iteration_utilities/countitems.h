#ifndef PYIU_COUNTITEMS_H
#define PYIU_COUNTITEMS_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_Count(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#endif
