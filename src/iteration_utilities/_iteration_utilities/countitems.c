/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "countitems.h"
#include "helper.h"

PyObject *
PyIU_Count(PyObject *Py_UNUSED(m),
           PyObject *args,
           PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "pred", "eq", NULL};

    PyObject *iterable;
    PyObject *item=NULL;
    PyObject *iterator=NULL;
    PyObject *val=NULL;
    PyObject *pred=NULL;
    Py_ssize_t sum_int = 0;
    int ok, eq=0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|Oi:count_items", kwlist,
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
        goto Fail;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }

    while ((item = Py_TYPE(iterator)->tp_iternext(iterator))) {

        /* No predicate given just set ok == 1 so the element is counted. */
        if (pred == NULL) {
            ok = 1;
            Py_DECREF(item);

        /* Always check for equality if "eq=1". */
        } else if (eq) {
            ok = PyObject_RichCompareBool(pred, item, Py_EQ);
            Py_DECREF(item);

        /* Predicate is bool, so we can skip the function call and just
           evaluate if the object is truthy.
           */
        } else if (pred == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);
            Py_DECREF(item);

        /* Call the function and check if the returned value is truthy. */
        } else {
            val = PyIU_CallWithOneArgument(pred, item);
            Py_DECREF(item);
            if (val == NULL) {
                goto Fail;
            }
            ok = PyObject_IsTrue(val);
            Py_DECREF(val);
        }

        /* If we found a match increment the counter, if we encountered an
           Exception throw it here.
           */
        if (ok == 1) {
            sum_int++;
        } else if (ok < 0) {
            goto Fail;
        }

        /* check if the sum variable is about to overflow. In this case there
           is no fallback because it's unlikely that we should process some
           iterable that's longer than the maximum py_ssize_t...
           */
        if (sum_int == PY_SSIZE_T_MAX) {
            PyErr_SetString(PyExc_TypeError,
                            "`iterable` for `count_items` is too long to count.");
            goto Fail;
        }
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            return NULL;
        }
    }

#if PY_MAJOR_VERSION == 2
    return PyInt_FromSsize_t(sum_int);
#else
    return PyLong_FromSsize_t(sum_int);
#endif

Fail:
    Py_XDECREF(iterator);
    return NULL;
}
