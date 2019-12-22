/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "minmax.h"
#include "helper.h"

#define SWAP(x, y)         \
    do {                   \
        PyObject *tmp = y; \
        y = x;             \
        x = tmp;           \
    } while (0)

PyObject *
PyIU_MinMax(PyObject *Py_UNUSED(m), PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"key", "default", NULL};
    PyObject *sequence;
    PyObject *iterator = NULL;
    PyObject *defaultitem = NULL;
    PyObject *keyfunc = NULL;
    PyObject *item1 = NULL;
    PyObject *maxitem = NULL;
    PyObject *maxval = NULL;
    PyObject *minitem = NULL;
    PyObject *minval = NULL;
    PyObject *resulttuple = NULL;
    int positional = PyTuple_GET_SIZE(args) > 1;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, "minmax", 1, 1, &sequence)) {
        return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(PyIU_global_0tuple, kwargs,
                                     "|OO:minmax", kwlist,
                                     &keyfunc, &defaultitem)) {
        return NULL;
    }
    if (keyfunc == Py_None) {
        keyfunc = NULL;
    }

    if (positional && defaultitem != NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "Cannot specify a default for `minmax` with multiple "
                        "positional arguments");
        goto Fail;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        goto Fail;
    }

    while ((item1 = Py_TYPE(iterator)->tp_iternext(iterator))) {
        PyObject *val1;
        PyObject *val2;
        PyObject *item2;
        int cmp;

        if (keyfunc != NULL) {
            val1 = PyIU_CallWithOneArgument(keyfunc, item1);
            if (val1 == NULL) {
                Py_DECREF(item1);
                goto Fail;
            }
        } else {
            Py_INCREF(item1);
            val1 = item1;
        }

        item2 = Py_TYPE(iterator)->tp_iternext(iterator);
        /* item2 could be NULL (end of sequence) clear a StopIteration but
           immediately fail if it's another exception. It will check for
           exceptions in the end (again) but make sure it does not process
           an iterable when the iterator threw an exception! */
        if (item2 == NULL) {
            if (PyIU_ErrorOccurredClearStopIteration()) {
                Py_DECREF(item1);
                Py_DECREF(val1);
                goto Fail;
            }
            Py_INCREF(item1);
            item2 = item1;
            Py_INCREF(val1);
            val2 = val1;
        } else {
            if (keyfunc != NULL) {
                val2 = PyIU_CallWithOneArgument(keyfunc, item2);
                if (val2 == NULL) {
                    Py_DECREF(item1);
                    Py_DECREF(val1);
                    Py_DECREF(item2);
                    goto Fail;
                }
            } else {
                val2 = item2;
                Py_INCREF(item2);
            }
        }

        /* maximum value and item are unset; set them. */
        if (minval == NULL) {
            Py_INCREF(item1);
            Py_INCREF(val1);
            Py_INCREF(item2);
            Py_INCREF(val2);
            minitem = item1;
            minval = val1;
            maxitem = item1;
            maxval = val1;

            item1 = item2;
            val1 = val2;
        }

        if (val1 != val2) {
            /* If both are set swap them if val2 is smaller than val1. */
            cmp = PyObject_RichCompareBool(val2, val1, Py_LT);
            if (cmp > 0) {
                SWAP(val1, val2);
                SWAP(item1, item2);
            } else if (cmp < 0) {
                Py_DECREF(item1);
                Py_DECREF(item2);
                Py_DECREF(val1);
                Py_DECREF(val2);
                goto Fail;
            }
        }

        /* val1 is smaller or equal to val2 so we compare only val1 with
            the current minimum.
            */
        cmp = PyObject_RichCompareBool(val1, minval, Py_LT);
        if (cmp > 0) {
            SWAP(minval, val1);
            SWAP(minitem, item1);
        } else if (cmp < 0) {
            Py_DECREF(item1);
            Py_DECREF(item2);
            Py_DECREF(val1);
            Py_DECREF(val2);
            goto Fail;
        }
        Py_DECREF(item1);
        Py_DECREF(val1);
        val1 = NULL;
        item1 = NULL;

        /* Same for maximum. */
        cmp = PyObject_RichCompareBool(val2, maxval, Py_GT);
        if (cmp > 0) {
            SWAP(maxval, val2);
            SWAP(maxitem, item2);
        } else if (cmp < 0) {
            Py_DECREF(item2);
            Py_DECREF(val2);
            goto Fail;
        }
        Py_DECREF(item2);
        Py_DECREF(val2);
    }
    Py_DECREF(iterator);
    iterator = NULL;

    if (PyIU_ErrorOccurredClearStopIteration()) {
        goto Fail;
    }

    if (minval == NULL) {
        if (maxval != NULL || minitem != NULL || maxitem != NULL) {
            /* This should be impossible to reach but better check. */
            goto Fail;
        }
        if (defaultitem != NULL) {
            minitem = defaultitem;
            maxitem = defaultitem;
            Py_INCREF(defaultitem);
            Py_INCREF(defaultitem);
        } else {
            PyErr_SetString(PyExc_ValueError,
                            "`minmax` `iterable` is an empty sequence");
            goto Fail;
        }
    } else {
        Py_DECREF(minval);
        Py_DECREF(maxval);
    }

    resulttuple = PyTuple_Pack(2, minitem, maxitem);
    Py_DECREF(minitem);
    Py_DECREF(maxitem);
    if (resulttuple == NULL) {
        return NULL;
    }

    return resulttuple;

Fail:
    Py_XDECREF(minval);
    Py_XDECREF(minitem);
    Py_XDECREF(maxval);
    Py_XDECREF(maxitem);
    Py_XDECREF(iterator);
    return NULL;
}
