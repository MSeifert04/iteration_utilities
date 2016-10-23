/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *keyfunc;
    PyObject *lastitem;
} PyIUObject_UniqueJust;

static PyTypeObject PyIUType_UniqueJust;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * uniquejust_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueJust *lz;

    PyObject *iterable, *iterator, *keyfunc=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_justseen", kwlist,
                                     &iterable, &keyfunc)) {
        return NULL;
    }

    /* Create and fill struct */
    if (keyfunc == Py_None) {
        keyfunc = NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    lz = (PyIUObject_UniqueJust *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iterator);
        Py_XDECREF(keyfunc);
        return NULL;
    }
    Py_XINCREF(keyfunc);
    lz->iterator = iterator;
    lz->keyfunc = keyfunc;
    lz->lastitem = NULL;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void uniquejust_dealloc(PyIUObject_UniqueJust *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->iterator);
    Py_XDECREF(lz->keyfunc);
    Py_XDECREF(lz->lastitem);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int uniquejust_traverse(PyIUObject_UniqueJust *lz, visitproc visit,
                               void *arg) {
    Py_VISIT(lz->iterator);
    Py_VISIT(lz->keyfunc);
    Py_VISIT(lz->lastitem);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * uniquejust_next(PyIUObject_UniqueJust *lz) {
    PyObject *item, *old, *val=NULL;
    int ok;

    while ( (item = (*Py_TYPE(lz->iterator)->tp_iternext)(lz->iterator)) ) {

        // Apply keyfunc or use the original
        if (lz->keyfunc == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            val = PyObject_CallFunctionObjArgs(lz->keyfunc, item, NULL);
            if (val == NULL) {
                goto Fail;
            }
        }

        // If no lastitem set it to the current and simply return the item.
        if (lz->lastitem == NULL) {
            lz->lastitem = val;
            return item;
        }

        // Otherwise compare it with the last item and only return it if it
        // differs
        ok = PyObject_RichCompareBool(val, lz->lastitem, Py_EQ);

        // Not equal
        if (ok == 0) {
            old = lz->lastitem;
            lz->lastitem = val;
            Py_DECREF(old);
            return item;

        // Failure
        } else if (ok < 0) {
            goto Fail;
        }

        Py_DECREF(val);
        Py_DECREF(item);
    }
    PYIU_CLEAR_STOPITERATION;
    return NULL;

Fail:
    Py_DECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * uniquejust_reduce(PyIUObject_UniqueJust *lz) {
    PyObject *value;

    if (lz->lastitem != NULL) {
        value = Py_BuildValue("O(OO)(O)", Py_TYPE(lz),
                             lz->iterator,
                             lz->keyfunc ? lz->keyfunc : Py_None,
                             lz->lastitem ? lz->lastitem : Py_None);
    } else {
        value = Py_BuildValue("O(OO)", Py_TYPE(lz),
                             lz->iterator,
                             lz->keyfunc ? lz->keyfunc : Py_None);
    }

    return value;
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * uniquejust_setstate(PyIUObject_UniqueJust *lz,
                                      PyObject *state) {
    PyObject *lastitem;

    if (!PyArg_ParseTuple(state, "O", &lastitem)) {
        return NULL;
    }

    Py_CLEAR(lz->lastitem);
    lz->lastitem = lastitem;
    Py_INCREF(lz->lastitem);

    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef uniquejust_methods[] = {
    {"__reduce__", (PyCFunction)uniquejust_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)uniquejust_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(uniquejust_doc, "unique_justseen(iterable[, key])\n\
\n\
List unique elements, preserving order. Remember only the element just seen.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` to check.\n\
\n\
key : callable or None, optional\n\
    If ``None`` the values are taken as they are. If it's a callable the\n\
    callable is applied to the value before comparing it.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
iterable : generator\n\
    An iterable containing all unique values just seen in the `iterable`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import unique_justseen\n\
>>> list(unique_justseen('AAAABBBCCDAABBB'))\n\
['A', 'B', 'C', 'D', 'A', 'B']\n\
\n\
>>> list(unique_justseen('ABBCcAD', str.lower))\n\
['A', 'B', 'C', 'A', 'D']");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_UniqueJust = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.unique_justseen", /* tp_name */
    sizeof(PyIUObject_UniqueJust),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)uniquejust_dealloc,     /* tp_dealloc */
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
    uniquejust_doc,                     /* tp_doc */
    (traverseproc)uniquejust_traverse,  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)uniquejust_next,      /* tp_iternext */
    uniquejust_methods,                 /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    uniquejust_new,                     /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
