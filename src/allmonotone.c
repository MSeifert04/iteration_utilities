/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject *
PyIU_Monotone(PyObject *m,
              PyObject *args,
              PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "decreasing", "strict", NULL};

    PyObject *iterable, *iterator, *item, *last=NULL;
    int decreasing=0, strict=0, op, ok;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ii:all_monotone", kwlist,
                                     &iterable, &decreasing, &strict)) {
        return NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    op = decreasing ? (strict ? Py_GT : Py_GE) : (strict ? Py_LT : Py_LE);

    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {
        if (last == NULL) {
            last = item;
            continue;
        }
        ok = PyObject_RichCompareBool(last, item, op);
        Py_DECREF(last);
        last = item;
        if (ok == 0) {
            goto Found;
        } else if (ok == -1) {
            goto Fail;
        }
    }

    PYIU_CLEAR_STOPITERATION;;

    Py_XDECREF(iterator);
    Py_XDECREF(last);
    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(last);
    return NULL;

Found:
    Py_XDECREF(iterator);
    Py_XDECREF(last);
    Py_RETURN_FALSE;
}

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(PyIU_Monotone_doc, "all_monotone(iterable, decreasing=False, strict=False)\n\
--\n\
\n\
Checks if the elements in `iterable` are (strictly) monotonic \n\
increasing or decreasing.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to test.\n\
\n\
decreasing : bool, optional\n\
    If ``False`` check if the values are monotonic increasing, otherwise\n\
    check for monotone decreasing.\n\
    Default is ``False``.\n\
\n\
strict : bool, optional\n\
    If ``True`` check if the elements are strictly greater or smaller\n\
    (``>`` or ``<``) than their predecessor. Otherwise use ``>=`` and ``<=``.\n\
\n\
Returns\n\
-------\n\
monotonic : :py:class:`bool`\n\
    ``True`` if all elements in `iterable` are monotonic or ``False`` if not.\n\
\n\
Notes\n\
-----\n\
If the input is empty the function returns ``True``.\n\
\n\
Examples\n\
--------\n\
This is roughly equivalent to\n\
``all(itertools.starmap(operator.lt, iteration_utilities.successive(iterable, 2)))``\n\
with the appropriate operator depending on `decreasing` and `strict`::\n\
\n\
    >>> from iteration_utilities import all_monotone\n\
    >>> all_monotone([1,1,1,1,1,1,1,1,1])\n\
    True\n\
    >>> all_monotone([1,1,1,1,1,1,1,1,1], strict=True)\n\
    False\n\
    >>> all_monotone([2,1,1,1,1,1,1,1,0], decreasing=True)\n\
    True\n\
    >>> all_monotone([2,1,1,1,1,1,1,1,0], decreasing=True, strict=True)\n\
    False");
