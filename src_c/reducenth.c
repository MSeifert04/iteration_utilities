static PyObject *
reduce_nth(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *iterable;
    Py_ssize_t n;
    PyObject *defaultitem=NULL;

    PyObject *iterator;
    Py_ssize_t i;
    PyObject *item=NULL;

    static char *kwlist[] = {"iterable", "n", "default", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "On|O:nth", kwlist,
                                     &iterable, &n, &defaultitem)) {
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    for (i=0 ; i<=n ; i++) {
        item = (*Py_TYPE(iterator)->tp_iternext)(iterator);
        if (item ==  NULL) {
            Py_DECREF(iterator);
            PyErr_Clear();
            if (defaultitem == NULL) {
                Py_DECREF(item);
                PyErr_Format(PyExc_IndexError, "not enough values.");
                return NULL;
            } else {
                Py_DECREF(item);
                Py_INCREF(defaultitem);
                return defaultitem;
            }
        }
        if (i != n) {
            Py_DECREF(item);
        }
    }
    Py_DECREF(iterator);
    return item;
}


PyDoc_STRVAR(reduce_nth_doc, "nth(iterable, n[, default])\n\
\n\
Returns the `n`-th item or a `default` value.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` from which to take the item.\n\
\n\
n : :py:class:`int`\n\
    Index of the item.\n\
\n\
default : any type, optional\n\
    `Default` value if the iterable doesn't contain the index.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
nth_item : any type\n\
    The `n`-th item of the `iterable` or `default` if the index wasn't\n\
    present in the `iterable`.\n\
\n\
Raises\n\
------\n\
IndexError\n\
    If the `iterable` contains less than `n` items and `default` is not given.\n\
\n\
Examples\n\
--------\n\
Without `default` value::\n\
\n\
    >>> from iteration_utilities import nth\n\
    >>> g = (x**2 for x in range(10))\n\
    >>> nth(g, 5)\n\
    25\n\
\n\
Or with `default` if the index is not present::\n\
\n\
    >>> g = (x**2 for x in range(10))\n\
    >>> nth(g, 15, 0)\n\
    0\n\
");
