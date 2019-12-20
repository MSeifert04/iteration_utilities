/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "alldistinct.h"
#include "helper.h"
#include "seen.h"

PyObject *
PyIU_AllDistinct(PyObject *Py_UNUSED(m), PyObject *iterable) {
    PyObject *iterator;
    PyObject *item;
    PyObject *seen;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    seen = PyIUSeen_New();
    if (seen == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    /* Almost identical to unique_everseen so no inline comments. */
    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {
        /* Check if the item is in seen. */
        int ok = PyIUSeen_ContainsAdd(seen, item);
        Py_DECREF(item);
        if (ok != 0) {
            /* Found duplicate or failure. */
            Py_DECREF(iterator);
            Py_DECREF(seen);
            if (ok == 1) {
                Py_RETURN_FALSE;
            } else if (ok == -1) {
                return NULL;
            }
        }
    }
    Py_DECREF(iterator);
    Py_DECREF(seen);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    Py_RETURN_TRUE;
}
