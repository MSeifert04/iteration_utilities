typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *it;
    PyObject *seen;
    PyObject *seenlist;
} recipes_uniqueever_object;

static PyTypeObject recipes_uniqueever_type;

static PyObject *
recipes_uniqueever_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "key", NULL};
    PyObject *func=NULL, *iterable=NULL, *seen=NULL, *seenlist=NULL;
    PyObject *it;
    recipes_uniqueever_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O:unique_everseen",
                                     kwargs, &iterable, &func)) {
        return NULL;
    }

    /* Get iterator. */
    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    seen = PySet_New(NULL);
    if (seen == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    /* create unique_everseen structure */
    lz = (recipes_uniqueever_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(seen);
        Py_DECREF(it);
        return NULL;
    }

    Py_XINCREF(func);
    lz->func = func;
    lz->it = it;
    lz->seen = seen;
    lz->seenlist = seenlist;

    return (PyObject *)lz;
}

static void
recipes_uniqueever_dealloc(recipes_uniqueever_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->seen);
    Py_XDECREF(lz->seenlist);
    Py_TYPE(lz)->tp_free(lz);
}

static int
recipes_uniqueever_traverse(recipes_uniqueever_object *lz, visitproc visit,
                            void *arg)
{
    Py_VISIT(lz->it);
    Py_VISIT(lz->func);
    Py_VISIT(lz->seen);
    Py_VISIT(lz->seenlist);
    return 0;
}

static PyObject *
recipes_uniqueever_next(recipes_uniqueever_object *lz)
{
    PyObject *item;
    PyObject *func = lz->func;
    PyObject *it = lz->it;
    PyObject *seen = lz->seen;
    PyObject *seenlist = lz->seenlist;
    PyObject *temp;
    long ok;
    PyObject *(*iternext)(PyObject *);

    iternext = *Py_TYPE(it)->tp_iternext;
    for (;;) {
        item = iternext(it);
        if (item == NULL)
            return NULL;

        if (func == NULL || func == Py_None) {
            temp = item;
            Py_INCREF(item);
        } else {
            temp = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (temp == NULL) {
                goto Fail;
            }
        }

        ok = PySet_Contains(seen, temp);

        if (ok == 0) {
            /* Item not found in set, add it and return */
            if (PySet_Add(seen, temp) == 0) {
                Py_DECREF(temp);
                return item;
            } else {
                goto Fail;
            }
        } else if (ok == 1) {
            /* item found in set, ignore it.*/
            Py_DECREF(temp);
            Py_DECREF(item);
        } else {
            /* Error when checking if the value is in the set.*/
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                } else {
                    goto Fail;
                }
            }


            // Create a list as fallback
            if (seenlist == NULL || seenlist == Py_None) {
                seenlist = PyList_New(0);
                if (seenlist == NULL) {
                    goto Fail;
                }
                // TODO: Not sure if this is actually needed ?!
                lz->seenlist = seenlist;
            }

            ok = seenlist->ob_type->tp_as_sequence->sq_contains(seenlist, temp);

            if (ok == 0) {
                /* Item not found in list, add it and return */
                if (PyList_Append(seenlist, temp) == 0) {
                    Py_DECREF(temp);
                    return item;
                } else {
                    goto Fail;
                }
            } else if (ok == 1) {
                /* item found in list, ignore it.*/
                Py_DECREF(temp);
                Py_DECREF(item);
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

static PyObject *
recipes_uniqueever_reduce(recipes_uniqueever_object *lz) {
    PyObject *value;
    value = Py_BuildValue("O(OO)(OO)", Py_TYPE(lz),
                          lz->it,
                          lz->func ? lz->func : Py_None,
                          lz->seen,
                          lz->seenlist ? lz->seenlist : Py_None);
    return value;
}

static PyObject *
recipes_uniqueever_setstate(recipes_uniqueever_object *lz, PyObject *state)
{
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

static PyMethodDef recipes_uniqueever_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_uniqueever_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_uniqueever_setstate,
     METH_O,
     ""},

    {NULL,              NULL}   /* sentinel */
};

PyDoc_STRVAR(recipes_uniqueever_doc,
"unique_everseen(sequence)\n\
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

static PyTypeObject recipes_uniqueever_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.unique_everseen", /* tp_name */
    sizeof(recipes_uniqueever_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_uniqueever_dealloc, /* tp_dealloc */
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
    recipes_uniqueever_doc,             /* tp_doc */
    (traverseproc)recipes_uniqueever_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_uniqueever_next, /* tp_iternext */
    recipes_uniqueever_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_uniqueever_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
