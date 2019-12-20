/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "anyisinstance.h"
#include "helper.h"

PyObject *
PyIU_AnyIsinstance(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "types", NULL};
    PyObject *iterable;
    PyObject *types;
    PyObject *iterator;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:any_isinstance", kwlist,
                                     &iterable, &types)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    for (;;) {
        int ok;
        PyObject *item = Py_TYPE(iterator)->tp_iternext(iterator);
        if (item == NULL) {
            break;
        }

        ok = PyObject_IsInstance(item, types);
        Py_DECREF(item);

        if (ok) {
            Py_DECREF(iterator);
            if (ok == 1) {
                Py_RETURN_TRUE;
            } else {
                return NULL;
            }
        }
    }

    Py_DECREF(iterator);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    Py_RETURN_FALSE;
}
