/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 *
 * IMPORTANT NOTE:
 *
 * The function does the same as "heapq.merge(*iterables)" or
 * "sorted(itertools.chain(*iterables))" it is included because heapq.merge
 * did not have the key and reverse parameter before Python 3.5 and it is
 * included for compatibility reasons.
 *
 * That this is faster than heapq.merge for most inputs is a nice but worrying
 * fact. :-(
 *
 *****************************************************************************/


typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    PyObject *keyfunc;
    PyObject *current;
    Py_ssize_t numactive;
    int reverse;
} PyIUObject_Merge;

static PyTypeObject PyIUType_Merge;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * merge_new(PyTypeObject *type, PyObject *args,
                            PyObject *kwargs) {
    PyIUObject_Merge *lz;

    PyObject *iteratortuple, *iterator, *keyfunc=NULL, *reversekw=NULL;
    Py_ssize_t numactive, idx, nkwargs;
    int reverse = Py_LT;

    /* Parse arguments */
    numactive = PyTuple_Size(args);

    if (kwargs != NULL && PyDict_Check(kwargs) && PyDict_Size(kwargs)) {
        nkwargs = 0;

        keyfunc = PyDict_GetItemString(kwargs, "key");
        if (keyfunc != NULL) {
            nkwargs++;
            Py_INCREF(keyfunc);
        }

        reversekw = PyDict_GetItemString(kwargs, "reverse");
        if (reversekw != NULL) {
            nkwargs++;
            if (PyObject_IsTrue(reversekw)) {
                reverse = Py_GT;
            }
        }

        if (PyDict_Size(kwargs) - nkwargs != 0) {
            PyErr_Format(PyExc_TypeError,
                         "merge got an unexpected keyword argument");
            Py_XDECREF(keyfunc);
            return NULL;
        }
    }

    /* Create and fill struct */
    iteratortuple = PyTuple_New(numactive);
    if (iteratortuple == NULL) {
        Py_XDECREF(keyfunc);
        return NULL;
    }
    for (idx=0 ; idx<numactive ; idx++) {
        iterator = PyObject_GetIter(PyTuple_GET_ITEM(args, idx));
        if (iterator == NULL) {
            Py_DECREF(iteratortuple);
            Py_XDECREF(keyfunc);
            return NULL;
        }
        PyTuple_SET_ITEM(iteratortuple, idx, iterator);
    }
    lz = (PyIUObject_Merge *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iteratortuple);
        Py_XDECREF(keyfunc);
        return NULL;
    }
    lz->iteratortuple = iteratortuple;
    lz->keyfunc = keyfunc;
    lz->reverse = reverse;
    lz->current = NULL;
    lz->numactive = numactive;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void merge_dealloc(PyIUObject_Merge *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->iteratortuple);
    Py_XDECREF(lz->keyfunc);
    Py_XDECREF(lz->current);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int merge_traverse(PyIUObject_Merge *lz, visitproc visit, void *arg) {
    Py_VISIT(lz->iteratortuple);
    Py_VISIT(lz->keyfunc);
    Py_VISIT(lz->current);
    return 0;
}

/******************************************************************************
 *
 * Initialize "current"
 *
 *****************************************************************************/

static int merge_init_current(PyIUObject_Merge *lz) {
    PyObject *current, *iterator, *item, *idx, *newitem, *keyval;
    Py_ssize_t i, insert, tuplelength;

    current = PyTuple_New(lz->numactive);
    if (current == NULL) {
        return -1;
    }
    tuplelength = 0;

    for (i=0 ; i<lz->numactive ; i++) {
        iterator = PyTuple_GET_ITEM(lz->iteratortuple, i);
        item = (*Py_TYPE(iterator)->tp_iternext)(iterator);
        if (item != NULL) {
            // Negate the index if we sort from high to small because this is
            // consistent with using Py_GT instead of Py_LT.
            if (lz->reverse) {
                idx = PyLong_FromSsize_t(-i);
            } else {
                idx = PyLong_FromSsize_t(i);
            }
            // The idea here is that we can keep stability by also remembering
            // the index of the iterable (which is also useful to remember
            // from which iterable to get the next item if it is yielded).
            // Using tuples allows to use the Python comparison operators which
            // takes the second item into account if the first is equal.
            // If a key function is given we make a tuple consisting of
            // [key(value), idx_iterator, value] otherwise [value, idx_iterator]
            if (lz->keyfunc == NULL) {
                newitem = PyTuple_Pack(2, item, idx);
            } else {
                keyval = PyObject_CallFunctionObjArgs(lz->keyfunc, item, NULL);
                if (keyval == NULL) {
                    Py_DECREF(current);
                    Py_DECREF(item);
                    Py_DECREF(idx);
                    return -1;
                }
                newitem = PyTuple_Pack(3, keyval, idx, item);
                Py_DECREF(keyval);
            }
            Py_DECREF(item);
            Py_DECREF(idx);

            // Insert the tuple into the current tuple.
            if (tuplelength==0) {
                PyTuple_SET_ITEM(current, 0, newitem);
            } else {
                insert = PyUI_TupleBisectRight(current, newitem, tuplelength,
                                               lz->reverse);
                if (insert < 0) {
                    Py_DECREF(current);
                    Py_DECREF(newitem);
                    return -1;
                }
                PYUI_TupleInsert(current, insert, newitem, tuplelength+1);
            }
            tuplelength++;
        } else {
            PYIU_CLEAR_STOPITERATION;
        }
    }
    lz->numactive = tuplelength;
    lz->current = current;
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * merge_next(PyIUObject_Merge *lz) {
    PyObject *iterator, *item, *val, *next, *keyval, *oldkeyval;
    Py_ssize_t idx, insert=0;

    // No current then we create one.
    if (lz->current == NULL || lz->current == Py_None) {
        if (merge_init_current(lz) < 0) {
            return NULL;
        }
    }

    // Finished as soon as there are no more active iterators.
    if (lz->numactive <= 0) {
        return NULL;
    }

    // Tuple containing the next value
    next = PyTuple_GET_ITEM(lz->current, lz->numactive-1);
    Py_INCREF(next);

    // Value to be returned
    if (lz->keyfunc == NULL) {
        val = PyTuple_GET_ITEM(next, 0);
    } else {
        val = PyTuple_GET_ITEM(next, 2);
    }
    Py_INCREF(val);

    // Iterable from which the value was taken
    idx = PyLong_AsSsize_t(PyTuple_GET_ITEM(next, 1));
    // Get the next value from the iterable where the value was from
    if (lz->reverse) {
        iterator = PyTuple_GET_ITEM(lz->iteratortuple, -idx);
    } else {
        iterator = PyTuple_GET_ITEM(lz->iteratortuple, idx);
    }
    item = (*Py_TYPE(iterator)->tp_iternext)(iterator);

    if (item == NULL) {
        PYIU_CLEAR_STOPITERATION;
        // No need to keep the extra reference for the tuple because there is
        // no successive value.
        Py_DECREF(next);
        Py_INCREF(val);
        lz->numactive--;
    } else {
        // Reuse the tuple, no need to set the idx again!
        // If no key is given the structure is [item, index_iterable]
        // otherwise [key(item), index_iterable, item]
        if (lz->keyfunc == NULL) {
            PyTuple_SET_ITEM(next, 0, item);
        } else {
            oldkeyval = PyTuple_GET_ITEM(next, 0);
            keyval = PyObject_CallFunctionObjArgs(lz->keyfunc, item, NULL);
            if (keyval == NULL) {
                Py_DECREF(item);
                Py_DECREF(val);
                Py_DECREF(next);
                return NULL;
            }
            PyTuple_SET_ITEM(next, 0, keyval);
            PyTuple_SET_ITEM(next, 2, item);
            Py_DECREF(oldkeyval);
        }

        // Insert the new value into the sorted current tuple.
        insert = PyUI_TupleBisectRight(lz->current, next, lz->numactive,
                                       lz->reverse);
        if (insert < 0) {
            Py_DECREF(next);
            Py_DECREF(next);
            Py_DECREF(val);
            Py_DECREF(val);
            return NULL;
        }
        PYUI_TupleInsert(lz->current, insert, next, lz->numactive);
        Py_DECREF(next);
    }
    Py_DECREF(val);
    return val;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * merge_reduce(PyIUObject_Merge *lz) {
    PyObject * res;
    res = Py_BuildValue("OO(OiOn)", Py_TYPE(lz),
                        lz->iteratortuple,
                        lz->keyfunc ? lz->keyfunc : Py_None,
                        lz->reverse,
                        lz->current ? lz->current : Py_None,
                        lz->numactive);
    return res;
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * merge_setstate(PyIUObject_Merge *lz, PyObject *state) {
    PyObject *current, *keyfunc;
    Py_ssize_t numactive;
    int reverse;
    if (!PyArg_ParseTuple(state, "OiOn",
                          &keyfunc, &reverse, &current, &numactive)) {
        return NULL;
    }

    Py_CLEAR(lz->current);
    lz->current = current;
    Py_INCREF(lz->current);

    if (keyfunc != Py_None) {
        Py_CLEAR(lz->keyfunc);
        lz->keyfunc = keyfunc;
        Py_INCREF(lz->keyfunc);
    }

    lz->numactive = numactive;
    lz->reverse = reverse;
    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef merge_methods[] = {
    {"__reduce__", (PyCFunction)merge_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)merge_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(merge_doc, "merge(*iterable, [key, reverse])\n\
\n\
Merge sorted `iterables` into one.\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    Any amount of already sorted `iterable` objects.\n\
\n\
key : callable or None, optional\n\
    If not given compare the item themselves otherwise compare the\n\
    result of ``key(item)``, like the `key` parameter for\n\
    :py:func:`sorted`.\n\
\n\
reverse : boolean, optional\n\
    If ``True`` then merge in decreasing order instead of increasing order.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
merged : generator\n\
    The merged iterables as generator.\n\
\n\
See also\n\
--------\n\
heapq.merge : Equivalent since Python 3.5 but only faster for a large number\n\
    (more than 5000) of iterables. Additionally earlier versions did not \n\
    support the `key` or `reverse` argument.\n\
\n\
sorted : ``sorted(itertools.chain(*iterables))`` supports the same options\n\
    and is much faster if you need a sequence instead of a generator.\n\
\n\
Examples\n\
--------\n\
To merge multiple sorted `iterables`::\n\
\n\
    >>> from iteration_utilities import merge\n\
    >>> list(merge([1, 3, 5, 7, 9], [2, 4, 6, 8, 10]))\n\
    [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]\n\
\n\
It's stable and allows a `key` function::\n\
\n\
    >>> seq1 = [(1, 3), (3, 3)]\n\
    >>> seq2 = [(-1, 3), (-3, 3)]\n\
    >>> list(merge(seq1, seq2, key=lambda x: abs(x[0])))\n\
    [(1, 3), (-1, 3), (3, 3), (-3, 3)]\n\
\n\
Also possible to `reverse` (biggest to smallest order) the merge::\n\
\n\
    >>> list(merge([5,1,-8], [10, 2, 1, 0], reverse=True))\n\
    [10, 5, 2, 1, 1, 0, -8]\n\
\n\
But also more than two `iterables`::\n\
\n\
    >>> list(merge([1, 10, 11], [2, 9], [3, 8], [4, 7], [5, 6], range(10)))\n\
    [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11]\n\
\n\
However if the `iterabes` are not sorted the result will be unsorted\n\
(partially sorted)::\n\
\n\
    >>> list(merge(range(10), [6,1,3,2,6,1,6]))\n\
    [0, 1, 2, 3, 4, 5, 6, 6, 1, 3, 2, 6, 1, 6, 7, 8, 9]");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Merge = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.merge",        /* tp_name */
    sizeof(PyIUObject_Merge),           /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)merge_dealloc,          /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    merge_doc,                          /* tp_doc */
    (traverseproc)merge_traverse,       /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)merge_next,           /* tp_iternext */
    merge_methods,                      /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    merge_new,                          /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
