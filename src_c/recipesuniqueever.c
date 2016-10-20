typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *iterator;
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

    PyObject *iterable, *iterator, *seen, *seenlist=NULL, *func=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_everseen", kwlist,
                                     &iterable, &func)) {
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    seen = PySet_New(NULL);
    if (seen == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    lz = (PyIUObject_UniqueEver *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iterator);
        Py_DECREF(seen);
        return NULL;
    }
    Py_XINCREF(func);
    lz->iterator = iterator;
    lz->func = func;
    lz->seen = seen;
    lz->seenlist = seenlist;
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
    Py_XDECREF(lz->func);
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
    Py_VISIT(lz->func);
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
    PyObject *item, *temp;
    long ok;

    for (;;) {
        item = (*Py_TYPE(lz->iterator)->tp_iternext)(lz->iterator);
        if (item == NULL) {
            PYIU_CLEAR_STOPITERATION;
            return NULL;
        }

        // Use the item if func is not given, otherwise apply the func.
        if (lz->func == NULL || lz->func == Py_None) {
            temp = item;
            Py_INCREF(item);
        } else {
            temp = PyObject_CallFunctionObjArgs(lz->func, item, NULL);
            if (temp == NULL) {
                goto Fail;
            }
        }

        // Check if the item is in the set
        ok = PySet_Contains(lz->seen, temp);

        // Not found
        if (ok == 0) {
            if (PySet_Add(lz->seen, temp) == 0) {
                Py_DECREF(temp);
                return item;
            } else {
                goto Fail;
            }

        // Found
        } else if (ok == 1) {
            Py_DECREF(temp);
            Py_DECREF(item);

        // Failure
        } else {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }

            // In case we got a TypeError we can still check if it's in the
            // list. This is much slower but works better than an exception. :-)
            if (lz->seenlist == NULL || lz->seenlist == Py_None) {
                lz->seenlist = PyList_New(0);
                if (lz->seenlist == NULL) {
                    goto Fail;
                }
            }

            ok = lz->seenlist->ob_type->tp_as_sequence->sq_contains(lz->seenlist, temp);

            // Not found
            if (ok == 0) {
                if (PyList_Append(lz->seenlist, temp) == 0) {
                    Py_DECREF(temp);
                    return item;
                } else {
                    goto Fail;
                }

            // Found
            } else if (ok == 1) {
                Py_DECREF(temp);
                Py_DECREF(item);

            // Failure
            } else {
                goto Fail;
            }
        }
    }

Fail:
    Py_XDECREF(temp);
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
                          lz->func ? lz->func : Py_None,
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

    Py_CLEAR(lz->seen);
    lz->seen = seen;
    Py_INCREF(lz->seen);
    Py_CLEAR(lz->seenlist);
    lz->seenlist = seenlist;
    Py_INCREF(lz->seenlist);
    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef uniqueever_methods[] = {
    {"__reduce__", (PyCFunction)uniqueever_reduce, METH_NOARGS, ""},
    {"__setstate__", (PyCFunction)uniqueever_setstate, METH_O, ""},
    {NULL, NULL}
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
Notes\n\
-----\n\
The items in the `iterable` must implement equality. If the items are hashable\n\
the function is much faster because the internally a ``set`` is used which\n\
speeds up the lookup if a value was seen.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import unique_everseen\n\
>>> list(unique_everseen('AAAABBBCCDAABBB'))\n\
['A', 'B', 'C', 'D']\n\
\n\
>>> list(unique_everseen('ABBCcAD', str.lower))\n\
['A', 'B', 'C', 'D']\n\
\n\
>>> list(unique_everseen([[1, 2], [1, 1], [1, 2]]))\n\
[[1, 2], [1, 1]]");

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
    0,                                  /* tp_getset */
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
