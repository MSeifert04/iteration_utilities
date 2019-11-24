#ifndef PYIU_ONE_H
#define PYIU_ONE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_One(PyObject *Py_UNUSED(m), PyObject *iterable);

#ifdef __cplusplus
}
#endif

#endif
