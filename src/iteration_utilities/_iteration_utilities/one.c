/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "one.h"
#include "helper.h"

PyObject *
PyIU_One(PyObject *Py_UNUSED(m), PyObject *iterable) {
    PyObject *iterator;
    PyObject *item1;
    PyObject *item2;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    item1 = Py_TYPE(iterator)->tp_iternext(iterator);
    if (item1 == NULL) {
        Py_DECREF(iterator);
        if (PyIU_ErrorOccurredClearStopIteration()) {
            return NULL;
        }
        PyErr_SetString(PyExc_ValueError,
                        "not enough values to unpack in `one` (expected 1, got 0)");
        return NULL;
    }

    item2 = Py_TYPE(iterator)->tp_iternext(iterator);
    if (item2 != NULL) {
        Py_DECREF(iterator);
        PyErr_Format(PyExc_ValueError,
                     "too many values to unpack in `one` (expected 1, got '%R, %R[, ...]').",
                     item1, item2);
        Py_DECREF(item1);
        Py_DECREF(item2);
        return NULL;
    }

    Py_DECREF(iterator);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        Py_DECREF(item1);
        return NULL;
    }
    return item1;
}
