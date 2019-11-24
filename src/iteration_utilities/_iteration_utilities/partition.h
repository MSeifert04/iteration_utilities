#ifndef PYIU_PARTITION_H
#define PYIU_PARTITION_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_Partition(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs);

#ifdef __cplusplus
}
#endif

#endif
