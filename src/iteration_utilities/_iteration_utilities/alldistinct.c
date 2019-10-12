/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "alldistinct.h"
#include "seen.h"

PyObject *
PyIU_AllDistinct(PyObject *Py_UNUSED(m),
                 PyObject *iterable)
{
    PyObject *iterator=NULL, *item=NULL, *seen=NULL;
    int ok;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    seen = PyIUSeen_New();
    if (seen == NULL) {
        goto Fail;
    }

    /* Almost identical to unique_everseen so no inline commments. */
    while ( (item = Py_TYPE(iterator)->tp_iternext(iterator)) ) {

        /* Check if the item is in seen. */
        ok = PyIUSeen_ContainsAdd(seen, item);
        if (ok == 1) {
            goto Found;
        /* Failure when looking for item.  */
        } else if (ok == -1) {
            goto Fail;
        }

        Py_DECREF(item);
    }

    Py_DECREF(iterator);
    Py_DECREF(seen);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            return NULL;
        }
    }

    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    Py_XDECREF(item);
    return NULL;

Found:
    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    Py_XDECREF(item);
    Py_RETURN_FALSE;
}
