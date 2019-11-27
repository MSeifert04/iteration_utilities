#ifndef PYIU_MATHEMATICAL_H
#define PYIU_MATHEMATICAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_MathSquare(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_MathDouble(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_MathReciprocal(PyObject *Py_UNUSED(m), PyObject *o);
PyObject * PyIU_MathRadd(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRsub(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRmul(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRdiv(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRfdiv(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRpow(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_MathRmod(PyObject *Py_UNUSED(m), PyObject *args);

#ifdef __cplusplus
}
#endif

#endif
