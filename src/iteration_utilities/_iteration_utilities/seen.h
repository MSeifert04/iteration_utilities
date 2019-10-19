#ifndef PYIU_SEEN_H
#define PYIU_SEEN_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *seenset;
    PyObject *seenlist;
} PyIUObject_Seen;

extern PyTypeObject PyIUType_Seen;

#define PyIUSeen_Check(o) (PyObject_TypeCheck(o, &PyIUType_Seen))
#define PyIUSeen_CheckExact(o) (Py_TYPE(o) == &PyIUType_Seen)

PyObject * PyIUSeen_New(void);
Py_ssize_t PyIUSeen_Size(PyIUObject_Seen *self);
int PyIUSeen_ContainsAdd(PyObject *self, PyObject *o);

#endif
