#ifndef PYIU_ANYISINSTANCE_H
#define PYIU_ANYISINSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_AnyIsinstance(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#ifdef __cplusplus
}
#endif

#endif
