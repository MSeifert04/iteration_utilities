#ifndef PYIU_ALLMONOTONE_H
#define PYIU_ALLMONOTONE_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_Monotone(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#endif
