static PyObject * PyIU_AllDistinct(PyObject *m, PyObject *iterable) {
    PyObject *iterator, *item, *seen, *seenlist=NULL;
    int ok;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    seen = PySet_New(NULL);
    if (seen == NULL) {
        goto Fail;
    }

    // Almost identical to unique_everseen so no inline commments.
    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {
        ok = PySet_Contains(seen, item);
        if (ok == 0) {
            ok = PySet_Add(seen, item);
            if (ok < 0) {
                goto Fail;
            }
        } else if (ok == 1) {
            goto Found;
        } else {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }
            if (seenlist == NULL) {
                seenlist = PyList_New(0);
                if (seenlist == NULL) {
                    goto Fail;
                }
            }
            ok = seenlist->ob_type->tp_as_sequence->sq_contains(seenlist, item);
            if (ok == 0) {
                ok = PyList_Append(seenlist, item);
                if (ok != 0) {
                    goto Fail;
                }
            } else if (ok == 1) {
                goto Found;
            } else {
                goto Fail;
            }
        }
        Py_DECREF(item);
    }

    PYIU_CLEAR_STOPITERATION;

    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);
    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);
    return NULL;

Found:
    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    Py_XDECREF(seenlist);
    Py_XDECREF(item);
    Py_RETURN_FALSE;
}

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(PyIU_AllDistinct_doc, "all_distinct(iterable)\n\
\n\
Checks if all items in the `iterable` are distinct.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
Returns\n\
-------\n\
distinct : bool\n\
    ``True`` if no two values are equal and ``False`` if there is at least\n\
    one duplicate in `iterable`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import all_distinct\n\
>>> all_distinct('AAAABBBCCDAABBB')\n\
False\n\
\n\
>>> all_distinct('abcd')\n\
True\n\
");
