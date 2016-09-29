static PyObject *
reduce_first(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *func = NULL;
    PyObject *item = NULL, *val = NULL;
    long ok;

    static char *kwlist[] = {"iterable", "default", "pred", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OO:first", kwlist,
                                     &sequence, &defaultitem, &func)) {
        return NULL;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        return NULL;
    }

    // Iterate over the sequence
    while (( item = PyIter_Next(iterator) )) {

        // Sequence contains an element and func is None: return it.
        if (func == NULL) {
            Py_DECREF(iterator);
            return item;

        } else if (func == Py_None || func == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);

        } else {
            val = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
            Py_DECREF(val);
        }

        if (ok == 1) {
            Py_DECREF(iterator);
            return item;

        } else if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            return NULL;
        }
        Py_DECREF(item);
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        return NULL;
    }

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_TypeError, "no first element found.");
        return NULL;
    // Otherwise return the default item
    } else {
        // Does it need to be incref'd?
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


PyDoc_STRVAR(reduce_first_doc, "first(iterable[, default, pred])\n\
\n\
Returns the first value in the `iterable` or `default`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to determine the first value.\n\
\n\
default : any type, optional\n\
    If no first value is found and `default` is given the `default` is \n\
    returned.\n\
\n\
pred : callable, optional\n\
    If given return the first item for which `pred` is ``True``.\n\
\n\
Returns\n\
-------\n\
first : any type\n\
    The first value or the first value for which `pred` is ``True``.\n\
    If there is no such value then `default` is returned.\n\
\n\
Raises\n\
-------\n\
TypeError :\n\
    If there is no first element and no `default` is given.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import first\n\
>>> first([0, '', tuple(), 10])\n\
0\n\
\n\
>>> first([0, '', tuple(), 10], pred=bool)\n\
10\n\
\n\
>>> # First odd number\n\
>>> first([0, 2, 3, 5, 8, 10], pred=lambda x: x%2)\n\
3\n\
\n\
>>> # default value if empty or no true value\n\
>>> first([], default=100)\n\
100\n\
>>> first([0, 0, 0, 0], pred=bool, default=100)\n\
100\n\
");
