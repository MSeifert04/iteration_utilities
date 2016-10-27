/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 *
 * IMPORTANT NOTE:
 *
 * This function is almost identical to "duplicates", so any changes
 * or bugfixes should also be implemented there!!!
 *
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *key;
    PyObject *seen;
    PyObject *seenlist;
} PyIUObject_UniqueEver;

static PyTypeObject PyIUType_UniqueEver;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * uniqueever_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueEver *lz;

    PyObject *iterable, *iterator, *seen, *key=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_everseen", kwlist,
                                     &iterable, &key)) {
        return NULL;
    }
    if (key == Py_None) {
        key = NULL;
    }

    /* Create and fill struct */
    if ( !(iterator = PyObject_GetIter(iterable)) ) {
        return NULL;
    }
    if ( !(seen = PySet_New(NULL)) ) {
        Py_DECREF(iterator);
        return NULL;
    }
    if ( !(lz = (PyIUObject_UniqueEver *)type->tp_alloc(type, 0)) ) {
        Py_DECREF(iterator);
        Py_DECREF(seen);
        return NULL;
    }
    Py_XINCREF(key);
    lz->iterator = iterator;
    lz->key = key;
    lz->seen = seen;
    lz->seenlist = NULL;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void uniqueever_dealloc(PyIUObject_UniqueEver *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->iterator);
    Py_XDECREF(lz->key);
    Py_XDECREF(lz->seen);
    Py_XDECREF(lz->seenlist);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int uniqueever_traverse(PyIUObject_UniqueEver *lz, visitproc visit,
                               void *arg) {
    Py_VISIT(lz->iterator);
    Py_VISIT(lz->key);
    Py_VISIT(lz->seen);
    Py_VISIT(lz->seenlist);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * uniqueever_next(PyIUObject_UniqueEver *lz) {
    PyObject *item=NULL, *temp=NULL;
    int ok;

    while ( (item = (*Py_TYPE(lz->iterator)->tp_iternext)(lz->iterator)) ) {

        // Use the item if key is not given, otherwise apply the key.
        if (lz->key == NULL) {
            temp = item;
        } else {
            if (!(temp = PyObject_CallFunctionObjArgs(lz->key, item, NULL))) {
                goto Fail;
            }
        }
        // Check if the item is in the set
        ok = PySet_Contains(lz->seen, temp);
        // Not found in the set
        if (ok == 0) {
            if (PySet_Add(lz->seen, temp) == 0) {
                goto Notfound;
            } else {
                goto Fail;
            }
        // Failure when looking if item is in the set
        } else if (ok < 0) {
            // TypeError probably means unhashable, so clear it raise others.
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }
            // Create the seenlist if its unset yet.
            if (lz->seenlist == NULL && !(lz->seenlist = PyList_New(0))) {
                goto Fail;
            }
            // Check if the item is in the list
            ok = lz->seenlist->ob_type->tp_as_sequence->sq_contains(lz->seenlist, temp);
            // Not found in the list either
            if (ok == 0) {
                if (PyList_Append(lz->seenlist, temp) == 0) {
                    goto Notfound;
                } else {
                    goto Fail;
                }
            // Failure when looking if item is in the list
            } else if (ok < 0) {
                goto Fail;
            }
        }
        // We have found the item either in the set or list so continue.
        if (lz->key != NULL) {
            Py_DECREF(temp);
        }
        Py_DECREF(item);
    }

    PYIU_CLEAR_STOPITERATION;
    return NULL;

Notfound:
    if (lz->key != NULL) {
        Py_XDECREF(temp);
    }
    return item;
Fail:
    if (lz->key != NULL) {
        Py_XDECREF(temp);
    }
    Py_XDECREF(item);
    return NULL;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * uniqueever_reduce(PyIUObject_UniqueEver *lz) {
    PyObject *value;
    value = Py_BuildValue("O(OO)(OO)", Py_TYPE(lz),
                          lz->iterator,
                          lz->key ? lz->key : Py_None,
                          lz->seen,
                          lz->seenlist ? lz->seenlist : Py_None);
    return value;
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * uniqueever_setstate(PyIUObject_UniqueEver *lz,
                                      PyObject *state) {
    PyObject *seen, *seenlist;

    if (!PyArg_ParseTuple(state, "OO", &seen, &seenlist)) {
        return NULL;
    }
    if (seenlist == Py_None) {
        seenlist = NULL;
    }

    Py_CLEAR(lz->seen);
    lz->seen = seen;
    Py_INCREF(lz->seen);
    Py_CLEAR(lz->seenlist);
    lz->seenlist = seenlist;
    Py_XINCREF(lz->seenlist);
    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef uniqueever_methods[] = {
    {"__reduce__",   (PyCFunction)uniqueever_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)uniqueever_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Seen property
 *
 *****************************************************************************/

static PyObject * uniqueever_getseen(PyIUObject_UniqueEver *lz,
                                     void *closure) {
    Py_INCREF(lz->seen);
    return lz->seen;
}

int uniqueever_setseen(PyIUObject_UniqueEver *lz, PyObject *o, void *closure) {
    if (o == NULL) {
        o = PySet_New(NULL);
        if (o == NULL) {
            return -1;
        }
    } else if (!PySet_Check(o)) {
        PyErr_Format(PyExc_TypeError, "`seen` must be a set-like object.");
        return -1;
    } else {
        Py_INCREF(o);
    }
    Py_DECREF(lz->seen);
    lz->seen = o;
    return 0;
}

/******************************************************************************
 *
 * Seenlist property
 *
 *****************************************************************************/

static PyObject * uniqueever_getseenlist(PyIUObject_UniqueEver *lz,
                                         void *closure) {
    if (lz->seenlist == NULL) {
        Py_RETURN_NONE;
    }
    Py_INCREF(lz->seenlist);
    return lz->seenlist;
}

int uniqueever_setseenlist(PyIUObject_UniqueEver *lz,  PyObject *o,
                           void *closure) {
    if (o == NULL || o == Py_None) {
        o = NULL;
    } else if (!PyList_Check(o)) {
        PyErr_Format(PyExc_TypeError, "`seenlist` must be a list-like object.");
        return -1;
    }
    Py_XDECREF(lz->seenlist);
    Py_XINCREF(o);
    lz->seenlist = o;
    return 0;
}

/******************************************************************************
 *
 * key property
 *
 *****************************************************************************/

static PyObject * uniqueever_getkey(PyIUObject_UniqueEver *lz, void *closure) {
    if (lz->key == NULL) {
        Py_RETURN_NONE;
    }
    Py_INCREF(lz->key);
    return lz->key;
}

int uniqueever_setkey(PyIUObject_UniqueEver *lz,  PyObject *o, void *closure) {
    if (o == NULL || o == Py_None) {
        o = NULL;
    }
    Py_XDECREF(lz->key);
    Py_XINCREF(o);
    lz->key = o;
    return 0;
}

/******************************************************************************
 *
 * Properties
 *
 *****************************************************************************/

static PyGetSetDef uniqueever_getsetlist[] = {
    {"seen",     (getter)uniqueever_getseen,     (setter)uniqueever_setseen,     NULL},
    {"seenlist", (getter)uniqueever_getseenlist, (setter)uniqueever_setseenlist, NULL},
    {"key",      (getter)uniqueever_getkey,      (setter)uniqueever_setkey,      NULL},
    {NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(uniqueever_doc, "unique_everseen(iterable[, key])\n\
\n\
List unique elements, preserving their order. Remembers all elements ever seen.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
key : callable, optional\n\
    If given it must be a callable taking one argument and this\n\
    callable is applied to the value before checking if it was seen yet.\n\
\n\
Returns\n\
-------\n\
iterable : generator\n\
    An iterable containing all unique values ever seen in the `iterable`.\n\
\n\
Attributes\n\
----------\n\
seen : set\n\
    Already seen (and hashable) values.\n\
seenlist : list, None\n\
    Already seen (and unhashable) values.\n\
key : callable, None\n\
    The key function.\n\
\n\
Notes\n\
-----\n\
The items in the `iterable` must implement equality. If the items are hashable\n\
the function is much faster because the internally a ``set`` is used which\n\
speeds up the lookup if a value was seen.\n\
\n\
Examples\n\
--------\n\
Some simple examples::\n\
\n\
    >>> from iteration_utilities import unique_everseen, consume\n\
    >>> list(unique_everseen('AAAABBBCCDAABBB'))\n\
    ['A', 'B', 'C', 'D']\n\
    \n\
    >>> list(unique_everseen('ABBCcAD', str.lower))\n\
    ['A', 'B', 'C', 'D']\n\
    \n\
Even unhashable values can be processed, like `list`::\n\
\n\
    >>> list(unique_everseen([[1, 2], [1, 1], [1, 2]]))\n\
    [[1, 2], [1, 1]]\n\
    \n\
One can access the already seen values by accessing the `seen` and `seenlist`\n\
attribute::\n\
\n\
    >>> uniques = unique_everseen(['a', [1, 2], 'b', [1, 1], [1, 2]])\n\
    >>> consume(uniques, 2)\n\
    >>> uniques.seen  # doctest: +SKIP\n\
    {'a'}\n\
    >>> uniques.seen == {'a'}\n\
    True\n\
    >>> uniques.seenlist\n\
    [[1, 2]]");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_UniqueEver = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.unique_everseen", /* tp_name */
    sizeof(PyIUObject_UniqueEver),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)uniqueever_dealloc,     /* tp_dealloc */
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
    uniqueever_doc,                     /* tp_doc */
    (traverseproc)uniqueever_traverse,  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)uniqueever_next,      /* tp_iternext */
    uniqueever_methods,                 /* tp_methods */
    0,                                  /* tp_members */
    uniqueever_getsetlist,              /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    uniqueever_new,                     /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
