static void
helper_tuple_insert(PyObject *tuple, Py_ssize_t where, PyObject *v, Py_ssize_t num)
{
    // Last item of the tuple MUST be NULL otherwise this leaves a dangling
    // reference!
    Py_ssize_t i;
    PyObject *temp;

    for ( i=num-2 ; i >= where ; i-- ) {
        temp = PyTuple_GET_ITEM(tuple, i);
        PyTuple_SET_ITEM(tuple, i+1, temp);
    }
    PyTuple_SET_ITEM(tuple, where, v);
}

Py_ssize_t
helper_bisect_right(PyObject *list, PyObject *item, Py_ssize_t hi, int cmpop)
{
    PyObject *litem;
    Py_ssize_t mid;
    int res;
    Py_ssize_t lo = 0;

    // It is likely that the next item of the yielded iterable is also the
    // next item to be yielded (?!) so check if it's fit to be included at the
    // end.
    litem = PyTuple_GET_ITEM(list, hi-1);
    if (litem == NULL) {
        return -1;
    }
    res = PyObject_RichCompareBool(item, litem, cmpop);
    if (res) {
        return hi;
    }

    hi--;

    while (lo < hi) {
        /* The (size_t)cast ensures that the addition and subsequent division
           are performed as unsigned operations, avoiding difficulties from
           signed overflow.  (See issue 13496.) */
        mid = ((size_t)lo + hi) / 2;
        litem = PyTuple_GET_ITEM(list, mid);
        if (litem == NULL) {
            return -1;
        }
        res = PyObject_RichCompareBool(item, litem, cmpop);
        if (res < 0) {
            return -1;
        }
        if (res) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}


typedef struct {
    PyObject_HEAD
    PyObject *ittuple;
    PyObject *keyfunc;
    int reverse;

    PyObject *current;
    Py_ssize_t numactive;
} recipes_merge_object;


static PyObject *
recipes_merge_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    recipes_merge_object *lz;

    PyObject *ittuple;
    PyObject *keyfunc = NULL;
    PyObject *reversekw = NULL;
    int reverse = Py_LT;

    Py_ssize_t numactive;

    PyObject *it;
    Py_ssize_t i, nkwds=0;

    if (!PyTuple_Check(args)) {
        PyErr_Format(PyExc_TypeError,
                     "`iterables` must be a tuple.");
        return NULL;
    }

    numactive = PyTuple_Size(args);

    ittuple = PyTuple_New(numactive);
    if (ittuple == NULL) {
        return NULL;
    }

    for (i=0 ; i<numactive ; i++) {
        it = PyObject_GetIter(PyTuple_GET_ITEM(args, i));
        if (it == NULL) {
            Py_DECREF(ittuple);
            return NULL;
        }
        PyTuple_SET_ITEM(ittuple, i, it);
    }

    if (kwds != NULL && PyDict_Check(kwds) && PyDict_Size(kwds)) {
        keyfunc = PyDict_GetItemString(kwds, "key");
        if (keyfunc != NULL) {
            nkwds++;
            Py_INCREF(keyfunc);
        }
        reversekw = PyDict_GetItemString(kwds, "reverse");
        if (reversekw != NULL) {
            nkwds++;
            if (PyObject_IsTrue(reversekw)) {
                reverse = Py_GT;
            }
        }
        if (PyDict_Size(kwds) - nkwds != 0) {
            PyErr_Format(PyExc_TypeError,
                         "merge got an unexpected keyword argument");
            Py_XDECREF(keyfunc);
            Py_DECREF(ittuple);
            return NULL;
        }
    }

    /* create recipes_merge_object structure */
    lz = (recipes_merge_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_XDECREF(keyfunc);
        Py_DECREF(ittuple);
        return NULL;
    }

    lz->ittuple = ittuple;
    lz->keyfunc = keyfunc;
    lz->reverse = reverse;
    lz->current = NULL;
    lz->numactive = numactive;

    return (PyObject *)lz;
}


static void
recipes_merge_dealloc(recipes_merge_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->ittuple);
    Py_XDECREF(lz->keyfunc);
    Py_XDECREF(lz->current);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_merge_traverse(recipes_merge_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->ittuple);
    Py_VISIT(lz->keyfunc);
    Py_VISIT(lz->current);
    return 0;
}


static int
recipes_merge_init_current(recipes_merge_object *lz)
{
    PyObject *ittuple = lz->ittuple;
    PyObject *current, *it, *item, *idx, *newitem, *keyval;
    Py_ssize_t i, insert;
    Py_ssize_t j=0;

    current = PyTuple_New(lz->numactive);
    if (current == NULL) {
        return -1;
    }

    for ( i=0 ; i < lz->numactive ; i++ ) {
        it = PyTuple_GET_ITEM(ittuple, i);
        item = PyIter_Next(it);
        if (item != NULL) {
            if (lz->reverse) {
                idx = PyLong_FromSsize_t(-i);
            } else {
                idx = PyLong_FromSsize_t(i);
            }
            // The idea here is that we can keep stability by also remembering
            // the index of the iterable (which is also useful to remember
            // from which iterable to get the next item if it is yielded).
            // Using tuples allows to use the Python comparison operators which
            // also take the second item into account.
            // If a key function is given we make a tuple consisting of
            // key(value) - index_iterable - value.
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

            if (j==0) {
                PyTuple_SET_ITEM(current, 0, newitem);
            } else {
                // Change if reverse is given
                insert = helper_bisect_right(current, newitem, j, lz->reverse);
                if (insert < 0) {
                    Py_DECREF(current);
                    Py_DECREF(newitem);
                    return -1;
                }
                helper_tuple_insert(current, insert, newitem, j+1);
            }
            j++;
        }
    }
    lz->numactive = j;
    lz->current = current;
    return 0;
}


static PyObject *
recipes_merge_next(recipes_merge_object *lz)
{
    PyObject *iterator, *item, *val, *next, *keyval, *oldkeyval;
    Py_ssize_t i, active=0, insert=0;

    // No current then we create one from the first elements of each iterable
    if (lz->current == NULL || lz->current == Py_None) {
        if (recipes_merge_init_current(lz) < 0) {
            return NULL;
        }
    }

    if (lz->numactive <= 0) {
        return NULL;
    }

    // Tuple containing the next value
    next = PyTuple_GET_ITEM(lz->current, lz->numactive-1);
    Py_INCREF(next);
    //PyTuple_SET_ITEM(lz->current, lz->numactive-1, NULL);
    // Value to be returned
    if (lz->keyfunc == NULL) {
        val = PyTuple_GET_ITEM(next, 0);
    } else {
        val = PyTuple_GET_ITEM(next, 2);
    }
    Py_INCREF(val);
    // Iterable from which the value was taken
    i = PyLong_AsSsize_t(PyTuple_GET_ITEM(next, 1));
    // Get the next value from the iterable where the value was from
    if (lz->reverse) {
        iterator = PyTuple_GET_ITEM(lz->ittuple, -i);
    } else {
        iterator = PyTuple_GET_ITEM(lz->ittuple, i);
    }
    item = PyIter_Next(iterator);

    if (item == NULL) {
        // No need to keep the extra reference for the tuple because there is
        // no successive value.
        Py_DECREF(next);
        Py_INCREF(val);
        lz->numactive--;
    } else {
        // Reuse the tuple (no need to set the idx again)
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

        // Change if reverse is given
        insert = helper_bisect_right(lz->current, next, lz->numactive, lz->reverse);
        if (insert < 0) {
            Py_DECREF(next);
            Py_DECREF(next);
            Py_DECREF(val);
            Py_DECREF(val);
            return NULL;
        }
        helper_tuple_insert(lz->current, insert, next, lz->numactive);
        Py_DECREF(next);
    }
    Py_DECREF(val);
    return val;
}

static PyObject *
recipes_merge_reduce(recipes_merge_object *lz)
{
    PyObject * res;
    res = Py_BuildValue("OO(OiOn)", Py_TYPE(lz),
                        lz->ittuple,
                        lz->keyfunc ? lz->keyfunc : Py_None,
                        lz->reverse,
                        lz->current ? lz->current : Py_None,
                        lz->numactive);
    return res;
}

static PyObject *
recipes_merge_setstate(recipes_merge_object *lz, PyObject *state)
{
    PyObject *current, *keyfunc;
    Py_ssize_t numactive;
    int reverse;
    if (!PyArg_ParseTuple(state, "OiOn", &keyfunc, &reverse, &current, &numactive)) {
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


static PyMethodDef recipes_merge_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_merge_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_merge_setstate,
     METH_O,
     ""},

    {NULL,           NULL}           /* sentinel */
};


PyDoc_STRVAR(recipes_merge_doc,
"merge(*iterable, [key, reverse])\n\
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
    [0, 1, 2, 3, 4, 5, 6, 6, 1, 3, 2, 6, 1, 6, 7, 8, 9]\n\
");

PyTypeObject recipes_merge_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.merge",        /* tp_name */
    sizeof(recipes_merge_object),       /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_merge_dealloc,  /* tp_dealloc */
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
    recipes_merge_doc,                  /* tp_doc */
    (traverseproc)recipes_merge_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_merge_next,   /* tp_iternext */
    recipes_merge_methods,              /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    recipes_merge_new,                  /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
