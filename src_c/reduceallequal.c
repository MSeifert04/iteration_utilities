static PyObject * PyIU_AllEqual(PyObject *m, PyObject *iterable) {
    PyObject *it=NULL;
    PyObject *(*iternext)(PyObject *);
    PyObject *item=NULL;
    PyObject *first=NULL;
    int ok;

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        goto Fail;
    }

    iternext = *Py_TYPE(it)->tp_iternext;
    while ( (item = iternext(it)) ) {
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

    PYIU_CLEAR_STOPITERATION;;

    Py_XDECREF(it);
    Py_XDECREF(first);

    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(it);
    Py_XDECREF(first);
    return NULL;

Found:
    Py_XDECREF(it);
    Py_XDECREF(first);
    Py_RETURN_FALSE;
}

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(PyIU_AllEqual_doc, "all_equal(iterable)\n\
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
False\n\
");
