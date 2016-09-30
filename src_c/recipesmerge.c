typedef struct {
    PyObject_HEAD
    PyObject *ittuple;
    PyObject *keyfunc;
    PyObject *reverse;

    PyObject *current;
    Py_ssize_t numactive;
} recipes_merge_object;


static PyObject *
recipes_merge_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{

    PyObject *ittuple;
    PyObject *keyfunc = NULL;
    PyObject *reverse = 0;

    PyObject *current = NULL;
    Py_ssize_t numactive;
    recipes_merge_object *lz;

    PyObject *item, *iterator;
    Py_ssize_t i, nkwds=0;

    assert(PyTuple_Check(args));
    numactive = PyTuple_Size(args);

    ittuple = PyTuple_New(numactive);
    if (ittuple == NULL) {
        return NULL;
    }

    for (i = 0 ; i < numactive ; i++) {
        item = PyTuple_GET_ITEM(args, i);
        iterator = PyObject_GetIter(item);
        if (iterator == NULL) {
            Py_DECREF(ittuple);
            return NULL;
        }
        PyTuple_SET_ITEM(ittuple, i, iterator);
    }

    if (kwds != NULL && PyDict_Check(kwds) && PyDict_Size(kwds)) {
        keyfunc = PyDict_GetItemString(kwds, "key");
        if (keyfunc != NULL) {
            nkwds++;
            Py_INCREF(keyfunc);
        }
        reverse = PyDict_GetItemString(kwds, "reverse");
        if (reverse != NULL) {
            nkwds++;
            Py_INCREF(reverse);
        }
        if (PyDict_Size(kwds) - nkwds != 0) {
            PyErr_Format(PyExc_TypeError,
                         "merge got an unexpected keyword argument");
            Py_XDECREF(keyfunc);
            Py_XDECREF(reverse);
            Py_DECREF(ittuple);
            return NULL;
        }
    }

    /* create recipes_merge_object structure */
    lz = (recipes_merge_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_XDECREF(keyfunc);
        Py_XDECREF(reverse);
        Py_DECREF(ittuple);
        return NULL;
    }

    lz->ittuple = ittuple;
    lz->keyfunc = keyfunc;
    lz->reverse = reverse;
    lz->current = current;
    lz->numactive = numactive;

    return (PyObject *)lz;
}


static void
recipes_merge_dealloc(recipes_merge_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->ittuple);
    Py_XDECREF(lz->keyfunc);
    Py_XDECREF(lz->reverse);
    Py_XDECREF(lz->current);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_merge_traverse(recipes_merge_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->ittuple);
    Py_VISIT(lz->keyfunc);
    Py_VISIT(lz->reverse);
    Py_VISIT(lz->current);
    return 0;
}

static void
helper_tuple_remove(PyObject *tuple, Py_ssize_t idx, Py_ssize_t num)
{
    Py_ssize_t i;
    PyObject *olditem, *temp;

    olditem = PyTuple_GET_ITEM(tuple, idx);
    Py_DECREF(olditem);

    for (i=idx+1 ; i < num ; i++) {
        olditem = PyTuple_GET_ITEM(tuple, i);
        Py_INCREF(olditem);
        PyTuple_SET_ITEM(tuple, i-1, olditem);
    }
    Py_DECREF(PyTuple_GET_ITEM(tuple, num-1));
}

static void
helper_tuple_insert(PyObject *tuple, Py_ssize_t where, PyObject *v, Py_ssize_t num)
{
    Py_ssize_t i;
    PyObject *temp;

    for (i = num; --i >= where; ) {
        temp = PyTuple_GET_ITEM(tuple, i);
        Py_INCREF(temp);
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


static PyObject *
recipes_merge_next(recipes_merge_object *lz)
{
    PyObject *ittuple = lz->ittuple;
    PyObject *current = lz->current;
    PyObject *keyfunc = lz->keyfunc;
    PyObject *reverse = lz->reverse;
    Py_ssize_t numactive = lz->numactive;

    PyObject *iterator, *item, *temp, *ret, *idx;
    Py_ssize_t i, active=0, insert=0;

    // No current then we create one from the first elements of each iterable
    if (current == NULL || current == Py_None) {
        current = PyTuple_New(numactive);
        if (current == NULL) {
            return NULL;
        }
        // Go through the iterables in reverse so we can remove them immediatly
        for (i=0 ; i<numactive ; i++) {
            iterator = PyTuple_GET_ITEM(ittuple, i);
            item = PyIter_Next(iterator);
            if (item == NULL) {
                helper_tuple_remove(ittuple, i, numactive);
                numactive--;
                i--;
            } else {
                idx = PyLong_FromSsize_t(i);
                temp = PyTuple_Pack(2, item, idx);  // Change if keyfunc is given
                if (i==0) {
                    PyTuple_SET_ITEM(current, 0, temp);
                } else {
                    insert = helper_bisect_right(current, temp, i, Py_LT);  // Change if reverse is given
                    if (insert < 0) {
                        return NULL;
                    }
                    helper_tuple_insert(current, insert, temp, i);
                }
                Py_DECREF(item);
                Py_DECREF(idx);
            }
        }
        lz->current = current;
    }

    if (numactive == 0) {
        return NULL;
    }

    item = PyTuple_GET_ITEM(current, numactive-1);
    ret = PyTuple_GET_ITEM(item, 0);
    idx = PyTuple_GET_ITEM(item, 1);
    i = PyLong_AsSsize_t(idx);
    iterator = PyTuple_GET_ITEM(ittuple, i);
    temp = PyIter_Next(iterator);
    if (temp == NULL) {
        Py_INCREF(ret);
        Py_DECREF(item);
        lz->numactive--;
    } else {
        PyTuple_SET_ITEM(item, 0, temp);
        insert = helper_bisect_right(current, item, numactive, Py_LT);  // Change if reverse is given
        if (insert < 0) {
            return NULL;
        }
        helper_tuple_insert(current, insert, item, numactive);
    }
    return ret;
}


static PyObject *
recipes_merge_reduce(recipes_merge_object *lz)
{
    PyObject *ittuple, *current, *temp, *res;
    Py_ssize_t i;

    if (PyTuple_Size(lz->ittuple) != lz->numactive) {
        ittuple = PyTuple_New(lz->numactive);
        if (ittuple == NULL) {
            return NULL;
        }
        for (i=0 ; i<lz->numactive ; i++) {
            temp = PyTuple_GET_ITEM(lz->ittuple, i);
            Py_INCREF(temp);
            PyTuple_SET_ITEM(ittuple, i, temp);
        }
    } else {
        ittuple = lz->ittuple;
        Py_INCREF(ittuple);
    }

    if (lz->current != Py_None && lz->current != NULL &&
            PyTuple_Size(lz->current) != lz->numactive) {
        current = PyTuple_New(lz->numactive);
        if (current == NULL) {
            return NULL;
        }
        for (i=0 ; i<lz->numactive ; i++) {
            temp = PyTuple_GET_ITEM(lz->current, i);
            Py_INCREF(temp);
            PyTuple_SET_ITEM(current, i, temp);
        }
    } else {
        current = lz->current;
        Py_INCREF(current);
    }

    res = Py_BuildValue("OO{s:O,s:O}(On)", Py_TYPE(lz),
                        ittuple,
                        "key", lz->keyfunc ? lz->keyfunc : Py_None,
                        "reverse", lz->reverse ? lz->reverse : Py_None,
                        current ? lz->current : Py_None, lz->numactive);
    Py_DECREF(ittuple);
    Py_DECREF(current);
    return res;
}

static PyObject *
recipes_merge_setstate(recipes_merge_object *lz, PyObject *state)
{
    PyObject *current;
    Py_ssize_t numactive;
    if (!PyArg_ParseTuple(state, "On", &current, &numactive)) {
        return NULL;
    }

    Py_CLEAR(lz->current);
    lz->current = current;
    Py_INCREF(lz->current);

    lz->numactive = numactive;

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
"merge(*iterable)\n\
\n\
Merge sorted `iterables` into one.\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    Any amount of already sorted `iterable` objects.\n\
\n\
key : callable or None, optional\n\
    If ``None`` compare the elements themselves otherwise compare the\n\
    result of ``key(element)``, like the `key` parameter for\n\
    :py:func:`sorted`.\n\
    Default is ``None``.\n\
\n\
reverse : boolean, optional\n\
    If ``True`` then sort decreasing otherwise sort in increasing order.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
merged : generator\n\
    The sorted merged iterables as generator.\n\
\n\
See also\n\
--------\n\
heapq.merge : Equivalent and faster since Python 3.5 but earlier versions\n\
    do not support the `key` or `reverse` argument.\n\
\n\
sorted : ``sorted(itertools.chain(*iterables))`` supports the same options\n\
    and is much faster but returns a sequence instead of a generator.\n\
\n\
Examples\n\
--------\n\
To merge multiple sorted `iterables`::\n\
\n\
    >>> from iteration_utilities import c_merge\n\
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
    "iteration_utilities.c_merge",   /* tp_name */
    sizeof(recipes_merge_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_merge_dealloc, /* tp_dealloc */
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
    recipes_merge_doc,             /* tp_doc */
    (traverseproc)recipes_merge_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_merge_next, /* tp_iternext */
    recipes_merge_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    recipes_merge_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};