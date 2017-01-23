/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject *
PyIU_AllDistinct(PyObject *m,
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
    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {

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

    PYIU_CLEAR_STOPITERATION;

    Py_XDECREF(iterator);
    Py_XDECREF(seen);
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

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(PyIU_AllDistinct_doc, "all_distinct(iterable, /)\n\
--\n\
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
Notes\n\
-----\n\
The items in the `iterable` should implement equality.\n\
\n\
If the items are hashable the function is much faster.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import all_distinct\n\
>>> all_distinct('AAAABBBCCDAABBB')\n\
False\n\
\n\
>>> all_distinct('abcd')\n\
True");
