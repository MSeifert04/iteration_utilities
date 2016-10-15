static PyObject * argminmax(PyObject *m, PyObject *args, PyObject *kwargs,
                            int cmpop){
    static char *kwlist[] = {"key", "default", NULL};
    PyObject *sequence;
    PyObject *keyfunc = NULL, *defaultvalue = NULL;
    PyObject *iterator = NULL;
    PyObject *(*iternext)(PyObject *);
    PyObject *item = NULL, *val = NULL;
    Py_ssize_t defaultitem = 0;
    int defaultisset = 0;
    Py_ssize_t idx = -1;
    PyObject *maxval = NULL;
    Py_ssize_t maxidx = -1;
    const int positional = PyTuple_Size(args) > 1;
    Py_ssize_t nkwargs = 0;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, "argmin/argmax", 1, 1, &sequence)) {
        return NULL;
    }

    if (kwargs != NULL && PyDict_Check(kwargs) && PyDict_Size(kwargs)) {
        keyfunc = PyDict_GetItemString(kwargs, "key");
        if (keyfunc != NULL) {
            nkwargs++;
            Py_INCREF(keyfunc);
        }
        defaultvalue = PyDict_GetItemString(kwargs, "default");
        if (defaultvalue != NULL) {
            nkwargs++;
#if PY_MAJOR_VERSION >= 3
            defaultitem = PyLong_AsSsize_t(defaultvalue);
#else
            // This will convert the value to an integer first so this differs
            // from the Py3 case.
            defaultitem = PyInt_AsSsize_t(defaultvalue);
#endif
            if (PyErr_Occurred()) {
                goto Fail;
            }
            defaultisset = 1;
        }
        if (PyDict_Size(kwargs) - nkwargs != 0) {
            PyErr_Format(PyExc_TypeError,
                         "argmin/argmax got an unexpected keyword argument");
            goto Fail;
        }
    }

    if (positional && defaultisset) {
        PyErr_Format(PyExc_TypeError,
                     "Cannot specify a default for argmin/argmax with multiple "
                     "positional arguments");
        goto Fail;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        goto Fail;
    }
    iternext = *Py_TYPE(iterator)->tp_iternext;

    // Iterate over the sequence
    while (( item = iternext(iterator) )) {
        idx++;
        if (keyfunc != NULL) {
            /* get the value from the key function */
            val = PyObject_CallFunctionObjArgs(keyfunc, item, NULL);
            if (val == NULL) {
                goto Fail;
            }
        } else {
            /* no key function; the value is the item */
            val = item;
            Py_INCREF(val);
        }

        if (maxval == NULL) { /* maximum value and item are unset; set them */
            maxval = val;
            maxidx = idx;
        } else { /* maximum value and item are set; update them as necessary */
            int cmpres = PyObject_RichCompareBool(val, maxval, cmpop);
            if (cmpres < 0) {
                goto Fail;
            } else if (cmpres > 0) {
                Py_DECREF(maxval);
                maxval = val;
                maxidx = idx;
            } else {
                Py_DECREF(val);
            }
        }
        Py_DECREF(item);
    }

    Py_DECREF(iterator);
    Py_XDECREF(maxval);
    Py_XDECREF(keyfunc);

    PYIU_CLEAR_STOPITERATION;

    if (PyErr_Occurred()) {
        goto Fail;
    }

    if (maxidx == -1) {
        if (defaultisset) {
            maxidx = defaultitem;
        } else {
            PyErr_Format(PyExc_ValueError, "argmin/argmax arg is an empty sequence");
            goto Fail;
        }
    }

    return PyLong_FromSsize_t(maxidx);

Fail:
    Py_XDECREF(keyfunc);
    Py_XDECREF(item);
    Py_XDECREF(val);
    Py_XDECREF(maxval);
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 *
 * Argmin
 *
 *****************************************************************************/

static PyObject * PyIU_Argmin(PyObject *self, PyObject *args, PyObject *kwargs) {
    return argminmax(self, args, kwargs, Py_LT);
}

/******************************************************************************
 *
 * Argmax
 *
 *****************************************************************************/

static PyObject * PyIU_Argmax(PyObject *self, PyObject *args, PyObject *kwargs) {
    return argminmax(self, args, kwargs, Py_GT);
}

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(PyIU_Argmin_doc, "argmin(iterable, *[, key, default])\n\
argmin(arg1, arg2, *iterable[, key])\n\
\n\
Find index of the minimum.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to calculate the index of the minimum.\n\
    Instead of passing the `iterable` the elements to compare can also be\n\
    given as (two or more) positional arguments.\n\
\n\
key : callable, optional\n\
    If not given then compare the values, otherwise compare ``key(item)``.\n\
\n\
default : int, optional\n\
    If not given raise ``ValueError`` if the `iterable` is empty otherwise\n\
    return ``default``\n\
\n\
Returns\n\
-------\n\
argmin : int\n\
    The index of the minimum or default if the `iterable` was empty.\n\
\n\
Examples\n\
--------\n\
For example::\n\
\n\
    >>> from iteration_utilities import argmin\n\
    >>> argmin(3,2,1,2,3)\n\
    2\n\
\n\
It allows a `key` function::\n\
\n\
    >>> argmin([3, -3, 0], key=abs)\n\
    2\n\
\n\
And a `default`::\n\
\n\
    >>> argmin([], default=10)\n\
    10\n\
");

PyDoc_STRVAR(PyIU_Argmax_doc, "argmax(iterable, *[, key, default])\n\
argmax(arg1, arg2, *iterable[, key])\n\
\n\
Find index of the maximum.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to calculate the index of the maximum.\n\
    Instead of passing the `iterable` the elements to compare can also be\n\
    given as (two or more) positional arguments.\n\
\n\
key : callable, optional\n\
    If not given then compare the values, otherwise compare ``key(item)``.\n\
\n\
default : int, optional\n\
    If not given raise ``ValueError`` if the `iterable` is empty otherwise\n\
    return ``default``\n\
\n\
Returns\n\
-------\n\
argmax : int\n\
    The index of the maximum or default if the `iterable` was empty.\n\
\n\
Examples\n\
--------\n\
For example::\n\
\n\
    >>> from iteration_utilities import argmax\n\
    >>> argmax(3,2,1,2,3)\n\
    0\n\
\n\
It allows a `key` function::\n\
\n\
    >>> argmax([0, -3, 3, 0], key=abs)\n\
    1\n\
\n\
And a `default`::\n\
\n\
    >>> argmax([], default=10)\n\
    10\n\
");