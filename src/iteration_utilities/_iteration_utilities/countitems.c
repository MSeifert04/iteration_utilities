/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "countitems.h"
#include "helper.h"

PyObject *
PyIU_Count(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "pred", "eq", NULL};
    PyObject *iterable;
    PyObject *item;
    PyObject *iterator;
    PyObject *pred = NULL;
    Py_ssize_t sum_int = 0;
    int eq = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Op:count_items", kwlist,
                                     &iterable, &pred, &eq)) {
        return NULL;
    }
    if (pred == Py_None) {
        pred = NULL;
    }

    if (eq && pred == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "`pred` argument for `count_items` must be specified "
                        "if `eq=True`.");
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {
        int ok;
        if (pred == NULL) {
            /* No predicate given just set ok == 1 so the element is counted. */
            ok = 1;
        } else if (eq) {
            /* Always check for equality if "eq=1". */
            ok = PyObject_RichCompareBool(pred, item, Py_EQ);
        } else if (pred == (PyObject *)&PyBool_Type) {
            /* Predicate is bool, so we can skip the function call and just
             evaluate if the object is truthy. */
            ok = PyObject_IsTrue(item);
        } else {
            /* Call the function and check if the returned value is truthy. */
            PyObject *val = PyIU_CallWithOneArgument(pred, item);
            if (val == NULL) {
                Py_DECREF(item);
                Py_DECREF(iterator);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
            Py_DECREF(val);
        }
        Py_DECREF(item);

        /* If we found a match increment the counter, if we encountered an
           Exception throw it here.
           */
        if (ok == 1) {
            /* check if the sum variable is about to overflow. In this case there
            is no fallback because it's unlikely that we should process some
            iterable that's longer than the maximum py_ssize_t...
            */
            if (sum_int == PY_SSIZE_T_MAX) {
                PyErr_SetString(PyExc_TypeError,
                                "`iterable` for `count_items` is too long to count.");
                Py_DECREF(iterator);
                return NULL;
            }
            sum_int++;
        } else if (ok < 0) {
            Py_DECREF(iterator);
            return NULL;
        }
    }

    Py_DECREF(iterator);

    if (PyIU_ErrorOccurredClearStopIteration()) {
        return NULL;
    }
    return PyLong_FromSsize_t(sum_int);
}
