/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "allequal.h"
#include "helper.h"

PyObject *
PyIU_AllEqual(PyObject *Py_UNUSED(m), PyObject *iterable) {
    PyObject *iterator;
    PyObject *item;
    PyObject *first = NULL;
    int ok;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {
        if (first == NULL) {
            first = item;
            continue;
        }
        ok = PyObject_RichCompareBool(first, item, Py_EQ);
        Py_DECREF(item);
        if (ok != 1) {
            Py_DECREF(iterator);
            Py_DECREF(first);
            if (ok == 0) {
                Py_RETURN_FALSE;
            } else if (ok == -1) {
                return NULL;
            }
        }
    }
    Py_DECREF(iterator);
    Py_XDECREF(first);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    Py_RETURN_TRUE;
}
