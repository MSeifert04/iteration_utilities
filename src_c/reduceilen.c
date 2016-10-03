static PyObject *
reduce_ilen(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", NULL};

    PyObject *iterable, *iterator, *item, *len;
    Py_ssize_t len_int = 0;
    int fallback = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:ilen",
                                     kwargs, &iterable)) {
        return NULL;
    }

    /* Get iterator. */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    // Fast version with integer increment
    while ((item = PyIter_Next(iterator))) {
        len_int++;
        if (len_int == PY_SSIZE_T_MAX) {
            fallback = 1;
            break;
        }
        Py_DECREF(item);
    }

    if (fallback > 0) {
        PyErr_Format(PyExc_TypeError,
                     "`iterable` is too long to compute the length.");
        return NULL;
    }

    len = PyLong_FromSsize_t(len_int);
    if (len == NULL) {
        return NULL;
    }

    return len;
}


PyDoc_STRVAR(reduce_ilen_doc, "ilen(iterable)\n\
\n\
Computes the `len` of an `iterable` by accessing all elements.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The length of this `iterable` will be determined.\n\
\n\
Returns\n\
-------\n\
length : int\n\
    The `length` of the `iterable`.\n\
\n\
Notes\n\
-----\n\
This function is able to determine the `length` of a generator-like object,\n\
but this will also exhaust the generator!\n\
\n\
Examples\n\
--------\n\
To compute the length of any iterable::\n\
\n\
    >>> from iteration_utilities import ilen\n\
    >>> ilen([1,2,3,4,5])\n\
    5\n\
\n\
    >>> len([1,2,3,4,5]) # here the builtin length could have been used too\n\
    5\n\
\n\
But it is also able to compute the length of a generator object but it will\n\
exhaust the generator while doing so::\n\
\n\
    >>> gen = (i for i in range(10))\n\
    >>> ilen(gen)\n\
    10\n\
    >>> ilen(gen)  # now the generator is empty\n\
    0\n\
\n\
So only use this if the _only_ thing you need to know about the generator is\n\
it's length.\n\
");

