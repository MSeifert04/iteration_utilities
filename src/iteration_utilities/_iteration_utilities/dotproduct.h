#ifndef PYIU_DOTPRODUCT_H
#define PYIU_DOTPRODUCT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_DotProduct(PyObject *Py_UNUSED(m), PyObject *args);

#ifdef __cplusplus
}
#endif

#endif
