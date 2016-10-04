static PyObject *
reduce_groupby(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "key", "keepkey", NULL};

    PyObject *iterable, *key1;  // mandatory arguments
    PyObject *key2=NULL;        // optional arguments

    PyObject *iterator, *item, *val, *lst, *keep;
    int ok;

    PyObject *resdict;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|O:groupby",
                                     kwargs, &iterable, &key1, &key2)) {
        return NULL;
    }

    /* Get iterator. */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    resdict = PyDict_New();
    if (resdict == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }

    while ( (item = PyIter_Next(iterator)) ) {
        val = PyObject_CallFunctionObjArgs(key1, item, NULL);
        if (val == NULL) {
            Py_DECREF(iterator);
            Py_DECREF(resdict);
            Py_DECREF(item);
            return NULL;
        }

        if (key2 == NULL || key2 == Py_None) {
            keep = item;
        } else {
            keep = PyObject_CallFunctionObjArgs(key2, item, NULL);
            if (keep == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(item);
                Py_DECREF(val);
                return NULL;
            }
            Py_DECREF(item);
        }

        lst = PyDict_GetItem(resdict, val);  // ignores any exception!!!
        if (lst == NULL) {
            lst = PyList_New(1);
            if (lst == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(keep);
                Py_DECREF(val);
                Py_DECREF(lst);
                return NULL;
            }

            PyList_SET_ITEM(lst, 0, keep);
            ok = PyDict_SetItem(resdict, val, lst);
            if (ok < 0) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(lst);
                Py_DECREF(val);
                return NULL;
            }
            Py_DECREF(val);
            Py_DECREF(lst);
        } else {
            Py_DECREF(val);
            ok = PyList_Append(lst, keep);
            if (ok < 0) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(keep);
                return NULL;
            }
            Py_DECREF(keep);
        }
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        Py_DECREF(resdict);
        return NULL;
    }

    return resdict;
}


PyDoc_STRVAR(reduce_groupby_doc, "groupby2(iterable, key[, keepkey])\n\
\n\
Group values of `iterable` by a `key` function.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` to group by a `key` function.\n\
\n\
key : callable\n\
    The items of the `iterable` are grouped by the ``key(item)``.\n\
\n\
keepkey : callable, optional\n\
    If given append only the result of ``keepkey(item)`` instead of ``item``.\n\
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
    >>> from iteration_utilities import groupby2\n\
    >>> from operator import itemgetter\n\
    >>> dct = groupby2(['a', 'bac', 'ba', 'ab', 'abc'], key=itemgetter(0))\n\
    >>> dct['a']\n\
    ['a', 'ab', 'abc']\n\
    >>> dct['b']\n\
    ['bac', 'ba']\n\
\n\
One could also specify a `keepkey`::\n\
\n\
    >>> dct = groupby2(['a', 'bac', 'ba', 'ab', 'abc'], key=itemgetter(0), keepkey=len)\n\
    >>> dct['a']\n\
    [1, 2, 3]\n\
    >>> dct['b']\n\
    [3, 2]\n\
\n\
");
