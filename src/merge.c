/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *
 * IMPORTANT NOTE:
 *
 * The function does the same as "heapq.merge(*iterables)" or
 * "sorted(itertools.chain(*iterables))" it is included because heapq.merge
 * did not have the key and reverse parameter before Python 3.5 and it is
 * included for compatibility reasons.
 *
 * That this is (much) faster than heapq.merge for most inputs is a nice but
 * worrying side effect. :-(
 *
 *****************************************************************************/


typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    PyObject *keyfunc;
    PyObject *current;
    Py_ssize_t numactive;
    int reverse;
    PyObject *funcargs;
} PyIUObject_Merge;

PyTypeObject PyIUType_Merge;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
merge_new(PyTypeObject *type,
          PyObject *args,
          PyObject *kwargs)
{
    PyIUObject_Merge *self;

    PyObject *iteratortuple, *iterator, *keyfunc=NULL, *reversekw=NULL, *funcargs=NULL;
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
    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        Py_DECREF(iteratortuple);
        Py_XDECREF(keyfunc);
        return NULL;
    }
    self = (PyIUObject_Merge *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(iteratortuple);
        Py_XDECREF(keyfunc);
        Py_XDECREF(funcargs);
        return NULL;
    }
    self->iteratortuple = iteratortuple;
    self->keyfunc = keyfunc;
    self->reverse = reverse;
    self->current = NULL;
    self->numactive = numactive;
    self->funcargs = funcargs;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
merge_dealloc(PyIUObject_Merge *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratortuple);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->current);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
merge_traverse(PyIUObject_Merge *self,
               visitproc visit,
               void *arg)
{
    Py_VISIT(self->iteratortuple);
    Py_VISIT(self->keyfunc);
    Py_VISIT(self->current);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Initialize "current"
 *****************************************************************************/

static int
merge_init_current(PyIUObject_Merge *self)
{
    PyObject *current, *iterator, *item, *newitem, *keyval=NULL, *tmp=NULL;
    Py_ssize_t i, insert, tuplelength;

    current = PyTuple_New(self->numactive);
    if (current == NULL) {
        return -1;
    }
    tuplelength = 0;

    for (i=0 ; i<self->numactive ; i++) {
        iterator = PyTuple_GET_ITEM(self->iteratortuple, i);
        item = (*Py_TYPE(iterator)->tp_iternext)(iterator);
        if (item != NULL) {
            /* The idea here is that we can keep stability by also remembering
               the index of the iterable (which is also useful to remember
               from which iterable to get the next item if it is yielded).
               */
            if (self->keyfunc != NULL) {
                PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, Py_DECREF(current); Py_DECREF(item); return -1;)
                keyval = PyObject_Call(self->keyfunc, self->funcargs, NULL);
                if (keyval == NULL) {
                    Py_DECREF(current);
                    Py_DECREF(item);
                    return -1;
                }
            }
            newitem = PyIU_ItemIdxKey_FromC(item, i, keyval);

            /* Insert the tuple into the current tuple. */
            if (tuplelength == 0) {
                PyTuple_SET_ITEM(current, 0, newitem);
            } else {
                insert = PyUI_TupleBisectRight_LastFirst(current, newitem,
                                                         tuplelength,
                                                         self->reverse);
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
    self->numactive = tuplelength;
    self->current = current;
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
merge_next(PyIUObject_Merge *self)
{
    PyObject *iterator, *item, *val, *keyval, *oldkeyval, *tmp=NULL;
    Py_ssize_t insert=0;
    PyIUObject_ItemIdxKey *next;

    /* No current then we create one. */
    if (self->current == NULL || self->current == Py_None) {
        if (merge_init_current(self) < 0) {
            return NULL;
        }
    }

    /* Finished as soon as there are no more active iterators. */
    if (self->numactive <= 0) {
        return NULL;
    }

    /* Tuple containing the next value. */
    next = (PyIUObject_ItemIdxKey *)PyTuple_GET_ITEM(self->current, self->numactive-1);
    Py_INCREF(next);

    /* Value to be returned. */
    val = next->item;
    Py_INCREF(val);

    /* Get the next value from the iterable where the value was from.  */
    iterator = PyTuple_GET_ITEM(self->iteratortuple, next->idx);
    item = (*Py_TYPE(iterator)->tp_iternext)(iterator);

    if (item == NULL) {
        PYIU_CLEAR_STOPITERATION;
        /* No need to keep the extra reference for the tuple because there is
           no successive value.
           */
        Py_DECREF(next);
        Py_INCREF(val);
        self->numactive--;
    } else {
        if (self->keyfunc != NULL) {
            oldkeyval = next->key;
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, Py_DECREF(item); Py_DECREF(val); Py_DECREF(next); return NULL;)
            keyval = PyObject_Call(self->keyfunc, self->funcargs, NULL);
            if (keyval == NULL) {
                Py_DECREF(item);
                Py_DECREF(val);
                Py_DECREF(next);
                return NULL;
            }
            next->key = keyval;
            next->item = item;
            Py_DECREF(oldkeyval);
        } else {
            next->item = item;
        }

        /* Insert the new value into the sorted current tuple. */
        insert = PyUI_TupleBisectRight_LastFirst(self->current, (PyObject *)next,
                                                 self->numactive-1, self->reverse);
        if (insert == -1) {
            Py_DECREF(next);
            Py_DECREF(next);
            Py_DECREF(val);
            Py_DECREF(val);
            return NULL;
        }
        PYUI_TupleInsert(self->current, insert, (PyObject *)next, self->numactive);
        Py_DECREF(next);
    }
    Py_DECREF(val);
    return val;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
merge_reduce(PyIUObject_Merge *self)
{
    PyObject * res;
    res = Py_BuildValue("OO(OiOn)", Py_TYPE(self),
                        self->iteratortuple,
                        self->keyfunc ? self->keyfunc : Py_None,
                        self->reverse,
                        self->current ? self->current : Py_None,
                        self->numactive);
    return res;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
merge_setstate(PyIUObject_Merge *self,
               PyObject *state)
{
    PyObject *current, *keyfunc;
    Py_ssize_t numactive;
    int reverse;
    if (!PyArg_ParseTuple(state, "OiOn",
                          &keyfunc, &reverse, &current, &numactive)) {
        return NULL;
    }

    Py_CLEAR(self->current);
    self->current = current;
    Py_INCREF(self->current);

    if (keyfunc != Py_None) {
        Py_CLEAR(self->keyfunc);
        self->keyfunc = keyfunc;
        Py_INCREF(self->keyfunc);
    }

    self->numactive = numactive;
    self->reverse = reverse;
    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
merge_lengthhint(PyIUObject_Merge *self)
{
    Py_ssize_t i, len = 0;
    if (self->current == NULL || self->current == Py_None) {
        for (i=0 ; i<PyTuple_Size(self->iteratortuple) ; i++) {
            len = len + PyObject_LengthHint(PyTuple_GET_ITEM(self->iteratortuple, i), 0);
        }
    } else {
        len = len + self->numactive;
        for (i=0 ; i<self->numactive ; i++) {
            len = len + PyObject_LengthHint(
                  PyTuple_GET_ITEM(self->iteratortuple,
                                   ((PyIUObject_ItemIdxKey *)PyTuple_GET_ITEM(self->current, i))->idx),
                  0);
        }
    }

    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef merge_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)merge_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__", (PyCFunction)merge_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)merge_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(merge_doc, "merge(*iterable, /, key=None, reverse=False)\n\
--\n\
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
heapq.merge : Equivalent since Python 3.5 but in most cases slower!\n\
    Earlier Python versions did not support the `key` or `reverse` argument.\n\
\n\
sorted : ``sorted(itertools.chain(*iterables))`` supports the same options\n\
    and *can* be faster.\n\
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
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Merge = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.merge",                        /* tp_name */
    sizeof(PyIUObject_Merge),                           /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)merge_dealloc,                          /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    merge_doc,                                          /* tp_doc */
    (traverseproc)merge_traverse,                       /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)merge_next,                           /* tp_iternext */
    merge_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    merge_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
