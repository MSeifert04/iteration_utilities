#ifndef PYIU_ARGMINMAX_H
#define PYIU_ARGMINMAX_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_Argmin(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);
PyObject * PyIU_Argmax(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#endif
