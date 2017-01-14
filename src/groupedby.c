/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject *
PyIU_Groupby(PyObject *m,
             PyObject *args,
             PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", "keep", "reduce", "reducestart", NULL};

    PyObject *iterable;
    PyObject *keyfunc;
    PyObject *valuefunc = NULL;
    PyObject *reducefunc = NULL;
    PyObject *valuedefault = NULL;

    PyObject *iterator = NULL;
    PyObject *resdict = NULL;
    PyObject *funcargs1 = NULL;
    PyObject *funcargs2 = NULL;
    PyObject *tmp1 = NULL;
    PyObject *tmp2 = NULL;
    PyObject *item = NULL;
    PyObject *key = NULL;
    PyObject *value = NULL;
    PyObject *valuetmp = NULL;
#if PY_MAJOR_VERSION == 2
    PyObject *setdefault = PyString_FromString("setdefault");
#endif
#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4
    PyObject *setdefault = PyUnicode_FromString("setdefault");
#endif
    int ok;

    /* Parse and validate input */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|OOO:groupby2", kwlist,
                                     &iterable, &keyfunc,
                                     &valuefunc, &reducefunc, &valuedefault)) {
        goto Fail;
    }
    if (reducefunc == NULL && valuedefault != NULL) {
        PyErr_Format(PyExc_TypeError,
                     "cannot specify `reducestart` if no `reduce` is given.");
        goto Fail;
    }

    /* Create necessary values */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    resdict = PyDict_New();
    if (resdict == NULL) {
        goto Fail;
    }
    funcargs1 = PyTuple_New(1);
    if (funcargs1 == NULL) {
        goto Fail;
    }
    if (reducefunc != NULL) {
        funcargs2 = PyTuple_New(2);
        if (funcargs2 == NULL) {
            goto Fail;
        }
    } else {
        valuedefault = PyList_New(0);
        if (valuedefault == NULL) {
            goto Fail;
        }
    }

    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {
        /* Calculate the key for the dictionary (val). */
        PYIU_RECYCLE_ARG_TUPLE(funcargs1, item, tmp1, goto Fail)
        key = PyObject_Call(keyfunc, funcargs1, NULL);
        if (key == NULL) {
            goto Fail;
        }

        /* Calculate the value for the dictionary (keep).  */
        if (valuefunc == NULL || valuefunc == Py_None) {
            value = item;
            item = NULL;
        } else {
            /* We use the same item again to calculate the keep so we don't need
               to replace. */
            //PYIU_RECYCLE_ARG_TUPLE(funcargs1, item, tmp1, goto Fail)
            value = PyObject_Call(valuefunc, funcargs1, NULL);
            Py_DECREF(item);
            item = NULL;
            if (value == NULL) {
                goto Fail;
            }
        }

        /* groupby operation */
        if (reducefunc == NULL) {
#if PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4)
            valuetmp = PyObject_CallMethodObjArgs(resdict, setdefault,
                                                  key, valuedefault, NULL);
            Py_XDECREF(valuetmp);
#else
            valuetmp = PyDict_SetDefault(resdict, key, valuedefault);
#endif
            Py_DECREF(key);
            key = NULL;

            if (valuetmp == NULL) {
                goto Fail;
            } else if (valuetmp == valuedefault) {
                valuedefault = PyList_New(0);
                if (valuedefault == NULL) {
                    goto Fail;
                }
            } else {
                Py_INCREF(valuetmp);
            }

            ok = PyList_Append(valuetmp, value);
            Py_DECREF(valuetmp);
            valuetmp = NULL;
            Py_DECREF(value);
            value = NULL;
            if (ok < 0) {
                goto Fail;
            }

        /* reduceby operation without default */
        } else if (valuedefault == NULL) {
#if PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4)
            valuetmp = PyObject_CallMethodObjArgs(resdict, setdefault,
                                                  key, value, NULL);
            Py_XDECREF(valuetmp);
#else
            valuetmp = PyDict_SetDefault(resdict, key, value);
#endif

            if (valuetmp == NULL) {
                goto Fail;
            } else if (valuetmp == value) {
                Py_DECREF(value);
                value = NULL;
                Py_DECREF(key);
                key = NULL;
            } else {
                /* In case of an error we need to incref valuetmp because it's
                   only borrowed */
                PYIU_RECYCLE_ARG_TUPLE_BINOP(funcargs2, valuetmp, value, tmp1, tmp2,
                                             Py_INCREF(valuetmp); goto Fail)
                valuetmp = PyObject_Call(reducefunc, funcargs2, NULL);
                if (valuetmp == NULL) {
                    goto Fail;
                }
                ok = PyDict_SetItem(resdict, key, valuetmp);
                if (ok < 0) {
                    goto Fail;
                }
                Py_DECREF(value);
                value = NULL;
                Py_DECREF(valuetmp);
                valuetmp = NULL;
                Py_DECREF(key);
                key = NULL;
            }

        /* reduceby operation with default */
        } else {
#if PY_MAJOR_VERSION == 2 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4)
            valuetmp = PyObject_CallMethodObjArgs(resdict, setdefault,
                                                  key, valuedefault, NULL);
            Py_XDECREF(valuetmp);
#else
            valuetmp = PyDict_SetDefault(resdict, key, valuedefault);
#endif

            if (valuetmp == NULL) {
                goto Fail;
            } else {
                /* In case of an error we need to incref valuetmp because it's
                   only borrowed */
                PYIU_RECYCLE_ARG_TUPLE_BINOP(funcargs2, valuetmp, value, tmp1, tmp2,
                                             Py_INCREF(valuetmp); goto Fail)
                valuetmp = PyObject_Call(reducefunc, funcargs2, NULL);
                if (valuetmp == NULL) {
                    goto Fail;
                }
                ok = PyDict_SetItem(resdict, key, valuetmp);
                if (ok < 0) {
                    goto Fail;
                }
                Py_DECREF(value);
                value = NULL;
                Py_DECREF(valuetmp);
                valuetmp = NULL;
                Py_DECREF(key);
                key = NULL;
            }
        }
    }

    if (reducefunc == NULL) {
        Py_DECREF(valuedefault);
    }
    Py_DECREF(iterator);
    Py_DECREF(funcargs1);
    Py_XDECREF(funcargs2);

    PYIU_CLEAR_STOPITERATION;

    if (PyErr_Occurred()) {
        Py_DECREF(resdict);
        return NULL;
    }

    return resdict;

Fail:
    if (reducefunc == NULL) {
        Py_XDECREF(valuedefault);
    }
    Py_XDECREF(iterator);
    Py_XDECREF(resdict);
    Py_XDECREF(funcargs1);
    Py_XDECREF(funcargs2);
    Py_XDECREF(tmp1);
    Py_XDECREF(tmp2);
    Py_XDECREF(item);
    Py_XDECREF(key);
    Py_XDECREF(value);
    Py_XDECREF(valuetmp);
    return NULL;
}

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(PyIU_Groupby_doc, "groupedby(iterable, key, keep=None, reduce=None, reducestart=None)\n\
--\n\
\n\
Group values of `iterable` by a `key` function as dictionary.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` to group by a `key` function.\n\
\n\
key : callable\n\
    The items of the `iterable` are grouped by the ``key(item)``.\n\
\n\
keep : callable, optional\n\
    If given append only the result of ``keep(item)`` instead of ``item``.\n\
\n\
reduce : callable, optional\n\
    If given then instead of returning a list of all ``items`` reduce them\n\
    using the binary `reduce` function. This works like the `func` parameter\n\
    in :py:func:`functools.reduce`.\n\
\n\
reducestart : any type, optional\n\
    Can only be specified if `reduce` is given. This parameter is equivalent\n\
    to the `start` parameter of :py:func:`functools.reduce`.\n\
\n\
Returns\n\
-------\n\
grouped : dict\n\
    A dictionary where the `keys` represent the ``key(item)`` and the `values`\n\
    are lists containing all ``items`` having the same `key`.\n\
\n\
Notes\n\
-----\n\
This function differs from ``itertools.groupy`` in several ways: (1) This\n\
function is eager (consumes the `iterable` in one go) and (2) the itertools\n\
function only groups the `iterable` locally.\n\
\n\
Examples\n\
--------\n\
A simple example::\n\
\n\
    >>> from iteration_utilities import groupedby\n\
    >>> from operator import itemgetter, add\n\
    >>> dct = groupedby(['a', 'bac', 'ba', 'ab', 'abc'], key=itemgetter(0))\n\
    >>> dct['a']\n\
    ['a', 'ab', 'abc']\n\
    >>> dct['b']\n\
    ['bac', 'ba']\n\
\n\
One could also specify a `keep` function::\n\
\n\
    >>> dct = groupedby(['a', 'bac', 'ba', 'ab', 'abc'], key=itemgetter(0), keep=len)\n\
    >>> dct['a']\n\
    [1, 2, 3]\n\
    >>> dct['b']\n\
    [3, 2]\n\
\n\
Or reduce all values for one key::\n\
\n\
    >>> from iteration_utilities import is_even\n\
    >>> dct = groupedby([1, 2, 3, 4, 5], key=is_even, reduce=add)\n\
    >>> dct[True]  # 2 + 4\n\
    6\n\
    >>> dct[False]  # 1 + 3 + 5\n\
    9\n\
\n\
using `reduce` also allows to specify a startvalue::\n\
\n\
    >>> dct = groupedby([1, 2, 3, 4, 5], key=is_even, reduce=add, reducestart=7)\n\
    >>> dct[True]  # 7 + 2 + 4\n\
    13\n\
    >>> dct[False]  # 7 + 1 + 3 + 5\n\
    16");
