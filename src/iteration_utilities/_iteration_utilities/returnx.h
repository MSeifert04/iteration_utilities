#ifndef PYIU_RETURNX_H
#define PYIU_RETURNX_H

#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_ReturnIdentity(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_ReturnCalled(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_ReturnFirstArg(PyObject *Py_UNUSED(m), PyObject *args, PyObject *Py_UNUSED(kwargs));

#endif
