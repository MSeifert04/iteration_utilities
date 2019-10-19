#ifndef PYIU_EXPORTEDHELPER_H
#define PYIU_EXPORTEDHELPER_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

PyObject * PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m), PyObject *args);

#endif
