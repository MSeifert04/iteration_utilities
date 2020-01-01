/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "always_iterable.h"
#include "helper.h"
#include "empty.h"

PyObject *
PyIU_AlwaysIterable(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"obj", "excluded_types", "empty_if_none", NULL};
    PyObject *object;
    PyObject *excluded_types = NULL;
    PyObject *tup;
    PyObject *result;
    int wrap_iterable = 0;
    int empty_if_none = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Op:always_iterable", kwlist,
                                     &object, &excluded_types, &empty_if_none)) {
        return NULL;
    }

    if (empty_if_none && object == Py_None) {
        Py_INCREF(PYIU_Empty);
        return PYIU_Empty;
    }

    if (excluded_types == NULL) {
        wrap_iterable = PyUnicode_CheckExact(object) || PyBytes_CheckExact(object);
    } else if (excluded_types != Py_None) {
        wrap_iterable = PyObject_IsInstance(object, excluded_types);
        if (wrap_iterable == -1) {
            return NULL;
        }
    }
    if (!wrap_iterable) {
        PyObject *it = PyObject_GetIter(object);
        if (it != NULL) {
            return it;
        }
        if (PyErr_Occurred()) {
            if (!PyErr_ExceptionMatches(PyExc_TypeError)) {
                return NULL;
            } else {
                PyErr_Clear();
            }
        }
    }
    tup = PyTuple_New(1);
    if (tup == NULL) {
        return NULL;
    }
    Py_INCREF(object);
    PyTuple_SET_ITEM(tup, 0, object);
    result = PyObject_GetIter(tup);
    Py_DECREF(tup);
    return result;
}
