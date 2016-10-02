static PyObject *
reduce_minmax(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *keyfunc = NULL;
    PyObject *item1 = NULL, *item2 = NULL, *val1 = NULL, *val2 = NULL;
    PyObject *maxitem = NULL, *maxval = NULL, *minitem = NULL, *minval = NULL;
    PyObject *temp = NULL, *emptytuple = NULL, *resulttuple = NULL;

    const int positional = PyTuple_Size(args) > 1;
    static char *kwlist[] = {"key", "default", NULL};
    int nkwds = 0;
    int ret;
    int cmp;

    if (positional) {
        sequence = args;
    } else if (!PyArg_UnpackTuple(args, "minmax", 1, 1, &sequence)) {
        return NULL;
    }

#if PY_MAJOR_VERSION >= 3
    emptytuple = PyTuple_New(0);
    if (emptytuple == NULL) {
        return NULL;
    }

    ret = PyArg_ParseTupleAndKeywords(emptytuple, kwds, "|$OO", kwlist,
                                      &keyfunc, &defaultitem);

    Py_DECREF(emptytuple);

    if (!ret) {
        return NULL;
    }
#else
    if (kwds != NULL && PyDict_Check(kwds) && PyDict_Size(kwds)) {
        keyfunc = PyDict_GetItemString(kwds, "key");
        if (keyfunc != NULL) {
            nkwds++;
            Py_INCREF(keyfunc);
        }
        defaultitem = PyDict_GetItemString(kwds, "default");
        if (defaultitem != NULL) {
            nkwds++;
            Py_INCREF(defaultitem);
        }
        if (PyDict_Size(kwds) - nkwds != 0) {
            PyErr_Format(PyExc_TypeError,
                         "minmax got an unexpected keyword argument");
            Py_XDECREF(keyfunc);
            Py_XDECREF(defaultitem);
            return NULL;
        }
    }
#endif

    if (positional && defaultitem != NULL) {
        PyErr_Format(PyExc_TypeError,
                     "Cannot specify a default for minmax with multiple "
                     "positional arguments");
#if PY_MAJOR_VERSION == 2
        Py_XDECREF(keyfunc);
        Py_XDECREF(defaultitem);
#endif
        return NULL;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
#if PY_MAJOR_VERSION == 2
        Py_XDECREF(keyfunc);
        Py_XDECREF(defaultitem);
#endif
        return NULL;
    }

    // Iterate over the sequence
    while (( item1 = PyIter_Next(iterator) )) {

        // It could be NULL (end of sequence) but don't care .. yet.
        item2 = PyIter_Next(iterator);

        /* get the value from the key function */
        if (keyfunc != NULL) {
            val1 = PyObject_CallFunctionObjArgs(keyfunc, item1, NULL);
            if (val1 == NULL) {
                goto Fail;
            }
            if (item2 != NULL) {
                val2 = PyObject_CallFunctionObjArgs(keyfunc, item2, NULL);
                if (val2 == NULL) {
                    goto Fail;
                }
            }
        }
        /* no key function; the value is the item */
        else {
            val1 = item1;
            Py_INCREF(val1);
            if (item2 != NULL) {
                val2 = item2;
                Py_INCREF(val2);
            }
        }

        /* maximum value and item are unset; set them */
        if (minval == NULL) {
            if (item2 != NULL) {
                // If both 1 and 2 are set do one compare and set min and max
                // accordingly
                cmp = PyObject_RichCompareBool(val1, val2, Py_LT);
                if (cmp < 0) {
                    goto Fail;
                } else if (cmp > 0) {
                    minval = val1;
                    minitem = item1;
                    maxval = val2;
                    maxitem = item2;
                } else {
                    // To keep stability we need to check if they are equal and
                    // only use val2 as minimum IF it's really smaller.
                    cmp = PyObject_RichCompareBool(val1, val2, Py_GT);
                    if (cmp < 0) {
                        // Should really be impossible because it already
                        // worked with LT but maybe we got some weird class
                        // here...
                        goto Fail;
                    } else if (cmp > 0) {
                        minval = val2;
                        minitem = item2;
                        maxval = val1;
                        maxitem = item1;
                    } else {
                        minval = val1;
                        minitem = item1;
                        maxval = val1;
                        maxitem = item1;
                        Py_DECREF(item2);
                        Py_DECREF(val2);
                    }
                }
            } else {
                // If only one is set we can set min and max to the only item.
                minitem = item1;
                minval = val1;
                maxitem = item1;
                maxval = val1;
            }
        } else {
            // If the sequence ends and only one element remains we can just
            // set item2/val2 to the last element and skip comparing these two.
            // This "should" keep the ordering, because it's unlikely someone
            // creates a type that is smallest and highest and uses minmax.
            if (item2 == NULL) {
                item2 = item1;
                val2 = val1;
                Py_INCREF(item1);
                Py_INCREF(val1);
            } else {
                // If both are set swap them if val2 is smaller than val1
                cmp = PyObject_RichCompareBool(val2, val1, Py_LT);
                if (cmp < 0) {
                    goto Fail;
                } else if (cmp > 0) {
                    temp = val1;
                    val1 = val2;
                    val2 = temp;

                    temp = item1;
                    item1 = item2;
                    item2 = temp;
                }
            }

            // val1 is smaller or equal to val2 so we compare only val1 with
            // the current minimum
            cmp = PyObject_RichCompareBool(val1, minval, Py_LT);
            if (cmp < 0) {
                goto Fail;
            } else if (cmp > 0) {
                Py_DECREF(minval);
                Py_DECREF(minitem);
                minval = val1;
                minitem = item1;
            } else {
                Py_DECREF(item1);
                Py_DECREF(val1);
            }

            // Same for maximum.
            cmp = PyObject_RichCompareBool(val2, maxval, Py_GT);
            if (cmp < 0) {
                goto Fail;
            } else if (cmp > 0) {
                Py_DECREF(maxval);
                Py_DECREF(maxitem);
                maxval = val2;
                maxitem = item2;
            } else {
                Py_DECREF(item2);
                Py_DECREF(val2);
            }
        }
    }

    if (PyErr_Occurred()) {
        goto Fail;
    }

#if PY_MAJOR_VERSION == 2
    Py_XDECREF(keyfunc);
#endif

    if (minval == NULL) {
        if (maxval != NULL || minitem != NULL || maxitem != NULL) {
            // This should be impossible to reach but better check.
            goto Fail;
        }
        if (defaultitem != NULL) {
            minitem = defaultitem;
            maxitem = defaultitem;
            Py_INCREF(defaultitem);
            Py_INCREF(defaultitem);
        } else {
            PyErr_Format(PyExc_ValueError, "minmax arg is an empty sequence");
            goto Fail;
        }
    } else {
        Py_DECREF(minval);
        Py_DECREF(maxval);
    }
#if PY_MAJOR_VERSION == 2
    Py_XDECREF(defaultitem);
#endif

    Py_DECREF(iterator);

    resulttuple = PyTuple_Pack(2, minitem, maxitem);

    Py_DECREF(minitem);
    Py_DECREF(maxitem);

    if (resulttuple == NULL) {
        goto Fail;
    }

    return resulttuple;

Fail:
#if PY_MAJOR_VERSION == 2
    Py_XDECREF(keyfunc);
    Py_XDECREF(defaultitem);
#endif
    Py_XDECREF(item1);
    Py_XDECREF(item2);
    Py_XDECREF(val1);
    Py_XDECREF(val2);
    Py_XDECREF(minval);
    Py_XDECREF(minitem);
    Py_XDECREF(maxval);
    Py_XDECREF(maxitem);
    Py_XDECREF(iterator);
    return NULL;
}


PyDoc_STRVAR(reduce_minmax_doc, "minmax(iterable, *[, key, default])\n\
minmax(arg1, arg2, *args[, key])\n\
\n\
Computes the minimum and maximum values in one-pass using only\n\
``1.5*len(iterable)`` comparisons. Recipe based on the snippet\n\
of Raymond Hettinger ([0]_) but significantly modified.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` for which to calculate the minimum and maximum.\n\
    Instead of passing the `iterable` the elements to compare can also be\n\
    given as (two or more) positional arguments.\n\
\n\
key : callable, optional\n\
    If not given then compare the values, otherwise compare ``key(item)``.\n\
\n\
default : any type, optional\n\
    If not given raise ``ValueError`` if the `iterable` is empty otherwise\n\
    return ``(default, default)``\n\
\n\
Returns\n\
-------\n\
minimum : any type\n\
    The `minimum` of the `iterable`.\n\
\n\
maximum : any type\n\
    The `maximum` of the `iterable`.\n\
\n\
Raises\n\
------\n\
ValueError\n\
    If `iterable` is empty and no `default` is given.\n\
\n\
See also\n\
--------\n\
min : Calculate the minimum of an iterable.\n\
\n\
max : Calculate the maximum of an iterable.\n\
\n\
Examples\n\
--------\n\
This function calculates the minimum (:py:func:`min`) and maximum\n\
(:py:func:`max`) of an `iterable`::\n\
\n\
    >>> from iteration_utilities import minmax\n\
    >>> minmax([2,1,3,5,4])\n\
    (1, 5)\n\
\n\
If the iterable is empty `default` is returned::\n\
\n\
    >>> minmax([], default=0)\n\
    (0, 0)\n\
\n\
Like the builtin functions it also supports a `key` argument::\n\
\n\
    >>> import operator\n\
    >>> seq = [(3, 2), (5, 1), (10, 3), (8, 5), (3, 4)]\n\
    >>> minmax(seq, key=operator.itemgetter(1))\n\
    ((5, 1), (8, 5))\n\
\n\
.. note::\n\
    This function is only faster if:\n\
\n\
    - A `key`-argument is given or\n\
    - Comparisons are costly or\n\
    - `iterable` is a generator.\n\
\n\
    In other cases using both :py:func:`min` and :py:func:`max` should be\n\
    preferred.\n\
\n\
References\n\
----------\n\
.. [0] http://code.activestate.com/recipes/577916/");

