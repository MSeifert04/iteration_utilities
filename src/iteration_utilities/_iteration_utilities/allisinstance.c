/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "allisinstance.h"
#include "helper.h"

PyObject *
PyIU_AllIsinstance(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "types", NULL};
    PyObject *iterable;
    PyObject *types;
    PyObject *iterator;
    PyObject *item;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:all_isinstance", kwlist,
                                     &iterable, &types)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {
        int ok = PyObject_IsInstance(item, types);
        Py_DECREF(item);
        if (ok != 1) {
            Py_DECREF(iterator);
            if (ok == 0) {
                Py_RETURN_FALSE;
            } else {
                return NULL;
            }
        }
    }
    Py_DECREF(iterator);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    Py_RETURN_TRUE;
}
