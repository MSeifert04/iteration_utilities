#ifndef PYIU_ALLDISTINCT_H
#define PYIU_ALLDISTINCT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_AllDistinct(PyObject *Py_UNUSED(m), PyObject *iterable);

#ifdef __cplusplus
}
#endif

#endif
