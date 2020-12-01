#ifndef PYIU_ITEMIDXKEY_H
#define PYIU_ITEMIDXKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *item;
    PyObject *key;
    Py_ssize_t idx;
} PyIUObject_ItemIdxKey;

extern PyTypeObject PyIUType_ItemIdxKey;

#define PyIU_ItemIdxKey_Check(o) PyObject_TypeCheck(o, &PyIUType_ItemIdxKey)

PyObject * PyIU_ItemIdxKey_FromC(PyObject *item, Py_ssize_t idx, PyObject *key);
PyObject * PyIU_ItemIdxKey_Copy(PyObject *iik);
int PyIU_ItemIdxKey_Compare(PyObject *v, PyObject *w, int op);

#ifdef __cplusplus
}
#endif

#endif
