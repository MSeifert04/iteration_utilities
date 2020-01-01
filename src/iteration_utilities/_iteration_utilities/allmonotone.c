/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "allmonotone.h"
#include "helper.h"

PyObject *
PyIU_Monotone(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "decreasing", "strict", NULL};
    PyObject *iterable;
    PyObject *iterator;
    PyObject *item;
    PyObject *last = NULL;
    int decreasing = 0;
    int strict = 0;
    int op;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|pp:all_monotone", kwlist,
                                     &iterable, &decreasing, &strict)) {
        return NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    op = decreasing ? (strict ? Py_GT : Py_GE) : (strict ? Py_LT : Py_LE);

    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {
        int ok;
        if (last == NULL) {
            last = item;
            continue;
        }
        ok = PyObject_RichCompareBool(last, item, op);
        Py_DECREF(last);
        last = item;
        if (ok != 1) {
            Py_DECREF(iterator);
            Py_DECREF(last);
            if (ok == 0) {
                Py_RETURN_FALSE;
            } else if (ok == -1) {
                return NULL;
            }
        }
    }

    Py_DECREF(iterator);
    Py_XDECREF(last);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    Py_RETURN_TRUE;
}
