/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

static PyObject *
PyIU_Groupby(PyObject *m,
             PyObject *args,
             PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", "keep", "reduce", "reducestart", NULL};

    PyObject *iterable, *key1, *key2=NULL, *iterator, *item, *val, *lst, *keep;
    PyObject *reduce=NULL, *reducestart=NULL, *reducetmp=NULL, *resdict;
    PyObject *funcargs1=NULL, *funcargs2=NULL, *tmp1=NULL, *tmp2=NULL;
    int ok;
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
    Py_hash_t hash;
#endif

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|OOO:groupby", kwlist,
                                     &iterable, &key1, &key2, &reduce,
                                     &reducestart)) {
        return NULL;
    }
    if (reduce == NULL && reducestart != NULL) {
        PyErr_Format(PyExc_TypeError,
                     "cannot specify `reducestart` if no `reduce` is given.");
        return NULL;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    resdict = PyDict_New();
    if (resdict == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    funcargs1 = PyTuple_New(1);
    if (funcargs1 == NULL) {
        Py_DECREF(iterator);
        Py_DECREF(resdict);
    }
    funcargs2 = PyTuple_New(2);
    if (funcargs2 == NULL) {
        Py_DECREF(funcargs1);
        Py_DECREF(iterator);
        Py_DECREF(resdict);
    }

    while ( (item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) ) {
        /* Calculate the key for the dictionary (val). */
        PYIU_RECYCLE_ARG_TUPLE(funcargs1, item, tmp1, Py_DECREF(item); goto Fail)
        val = PyObject_Call(key1, funcargs1, NULL);
        if (val == NULL) {
            Py_DECREF(item);
            goto Fail;
        }

        /* Calculate the value for the dictionary (keep).  */
        if (key2 == NULL || key2 == Py_None) {
            keep = item;
        } else {
            /* We use the same item again to calculate the keep so we don't need
               to replace. */
            //PYIU_RECYCLE_ARG_TUPLE(funcargs1, item, tmp1, Py_DECREF(item); goto Fail)
            keep = PyObject_Call(key2, funcargs1, NULL);
            Py_DECREF(item);
            if (keep == NULL) {
                Py_DECREF(val);
                goto Fail;
            }
        }

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
        /* Taken from dictobject.c CPython 3.5 */
        if (!PyUnicode_CheckExact(val) ||
                (hash = ((PyASCIIObject *) val)->hash) == -1) {
            hash = PyObject_Hash(val);
            if (hash == -1) {
                Py_DECREF(keep);
                Py_DECREF(val);
                goto Fail;
            }
        }
#endif

        /* Keep all values as list.  */
        if (reduce == NULL) {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
            lst = _PyDict_GetItem_KnownHash(resdict, val, hash);
#else
            lst = PyDict_GetItem(resdict, val);
#endif
            if (lst == NULL) {
                lst = PyList_New(1);
                if (lst == NULL) {
                    Py_DECREF(keep);
                    Py_DECREF(val);
                    goto Fail;
                }
                PyList_SET_ITEM(lst, 0, keep);
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, lst, hash);
#else
                ok = PyDict_SetItem(resdict, val, lst);
#endif
                Py_DECREF(lst);
                Py_DECREF(val);
                if (ok == -1) {
                    goto Fail;
                }
            } else {
                Py_DECREF(val);
                ok = PyList_Append(lst, keep);
                Py_DECREF(keep);
                if (ok < 0) {
                    goto Fail;
                }
            }

        /* Reduce the values with a binary operation. */
        } else {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
            lst = _PyDict_GetItem_KnownHash(resdict, val, hash);
#else
            lst = PyDict_GetItem(resdict, val);
#endif
            Py_XINCREF(lst);

            /* No item yet and no starting value given: Keep the "keep". */
            if (lst == NULL && reducestart == NULL) {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, keep, hash);
#else
                ok = PyDict_SetItem(resdict, val, keep);
#endif
                Py_DECREF(val);
                Py_DECREF(keep);
                if (ok == -1) {
                    goto Fail;
                }

            /* Already an item present so use the binary operation. */
            } else {
                if (lst == NULL) {
                    PYIU_RECYCLE_ARG_TUPLE_BINOP(funcargs2, reducestart, keep, tmp1, tmp2, Py_DECREF(keep);
                                                                                           goto Fail)
                    reducetmp = PyObject_Call(reduce, funcargs2, NULL);
                } else {
                    PYIU_RECYCLE_ARG_TUPLE_BINOP(funcargs2, lst, keep, tmp1, tmp2, Py_DECREF(keep);
                                                                                   Py_DECREF(lst);
                                                                                   goto Fail)
                    reducetmp = PyObject_Call(reduce, funcargs2, NULL);
                    Py_DECREF(lst);
                }
                Py_DECREF(keep);
                if (reducetmp == NULL) {
                    Py_DECREF(val);
                    goto Fail;
                }
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
                ok = _PyDict_SetItem_KnownHash(resdict, val, reducetmp, hash);
#else
                ok = PyDict_SetItem(resdict, val, reducetmp);
#endif
                Py_DECREF(val);
                Py_DECREF(reducetmp);
                if (ok == -1) {
                    goto Fail;
                }
            }
        }
    }

    Py_DECREF(funcargs1);
    Py_DECREF(funcargs2);

    PYIU_CLEAR_STOPITERATION;
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        Py_DECREF(resdict);
        return NULL;
    }

    return resdict;

Fail:
    Py_XDECREF(funcargs1);
    Py_XDECREF(funcargs2);
    Py_XDECREF(iterator);
    Py_XDECREF(resdict);
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
