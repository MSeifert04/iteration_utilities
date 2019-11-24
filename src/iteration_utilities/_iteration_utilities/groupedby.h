#ifndef PYIU_GROUPEDBY_H
#define PYIU_GROUPEDBY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_Groupby(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#ifdef __cplusplus
}
#endif

#endif
