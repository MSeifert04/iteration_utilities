#ifndef PYIU_RETURNX_H
#define PYIU_RETURNX_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_ReturnIdentity(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_ReturnCalled(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_ReturnFirstArg(PyObject *Py_UNUSED(m), PyObject *args, PyObject *Py_UNUSED(kwargs));

#ifdef __cplusplus
}
#endif

#endif
