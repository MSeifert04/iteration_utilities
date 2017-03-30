/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_AllEqual(PyObject *m,
              PyObject *iterable)
{
    PyObject *iterator, *item, *first=NULL;
    int ok;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }

    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {
        if (first == NULL) {
            first = item;
            continue;
        }
        ok = PyObject_RichCompareBool(first, item, Py_EQ);
        Py_DECREF(item);
        if (ok == 0) {
            goto Found;
        } else if (ok == -1) {
            goto Fail;
        }
    }
    Py_XDECREF(iterator);
    Py_XDECREF(first);

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
    Py_XDECREF(first);
    return NULL;

Found:
    Py_XDECREF(iterator);
    Py_XDECREF(first);
    Py_RETURN_FALSE;
}

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(PyIU_AllEqual_doc, "all_equal(iterable, /)\n\
--\n\
\n\
Checks if all the elements are equal to each other.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to test.\n\
\n\
Returns\n\
-------\n\
all_equal : :py:class:`bool`\n\
    ``True`` if all elements in `iterable` are equal or ``False`` if not.\n\
\n\
Notes\n\
-----\n\
If the input is empty the function returns ``True``.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import all_equal\n\
>>> all_equal([1,1,1,1,1,1,1,1,1])\n\
True\n\
\n\
>>> all_equal([1,1,1,1,1,1,1,2,1])\n\
False");
