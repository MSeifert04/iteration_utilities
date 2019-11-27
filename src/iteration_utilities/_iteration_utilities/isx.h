#ifndef PYIU_ISX_H
#define PYIU_ISX_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_IsNone(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_IsNotNone(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_IsEven(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_IsOdd(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_IsIterable(PyObject *Py_UNUSED(m), PyObject *o);

#ifdef __cplusplus
}
#endif

#endif
