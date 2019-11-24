#ifndef PYIU_EXPORTEDHELPER_H
#define PYIU_EXPORTEDHELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

#if PyIU_USE_VECTORCALL
PyObject * PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m), PyObject *const *args, size_t nargs);
PyObject * PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m), PyObject *const *args, size_t nargs);
#else
PyObject * PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m), PyObject *args);
PyObject * PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m), PyObject *args);
#endif

#ifdef __cplusplus
}
#endif

#endif
