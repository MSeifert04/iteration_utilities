typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    Py_ssize_t numactive;
    Py_ssize_t active;
} PyIUObject_Roundrobin;

static PyTypeObject PyIUType_Roundrobin;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * roundrobin_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    PyIUObject_Roundrobin *lz;

    PyObject *iteratortuple, *item, *iterator;
    Py_ssize_t numactive, idx;

    /* Parse arguments */
    numactive = PyTuple_Size(args);

    /* Create and fill struct */
    iteratortuple = PyTuple_New(numactive);
    if (iteratortuple == NULL) {
        return NULL;
    }
    for (idx = 0 ; idx<numactive ; idx++) {
        item = PyTuple_GET_ITEM(args, idx);
        iterator = PyObject_GetIter(item);
        if (iterator == NULL) {
            Py_DECREF(iteratortuple);
            return NULL;
        }
        PyTuple_SET_ITEM(iteratortuple, idx, iterator);
    }
    lz = (PyIUObject_Roundrobin *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iteratortuple);
        return NULL;
    }
    lz->iteratortuple = iteratortuple;
    lz->numactive = numactive;
    lz->active = 0;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void roundrobin_dealloc(PyIUObject_Roundrobin *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->iteratortuple);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int roundrobin_traverse(PyIUObject_Roundrobin *lz, visitproc visit,
                               void *arg) {
    Py_VISIT(lz->iteratortuple);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * roundrobin_next(PyIUObject_Roundrobin *lz) {
    PyObject *iterator, *item;

    // Stop if there is no active iterator left
    if (lz->numactive == 0) {
        return NULL;
    }

    iterator = PyTuple_GET_ITEM(lz->iteratortuple, lz->active);
    while ((item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) == NULL) {
        PYIU_CLEAR_STOPITERATION;
        // If the last iterable in the iteratortuple is empty simply set it to
        // NULL and reset the active pointer to 0.
        if (lz->active == lz->numactive-1) {
            PyTuple_SET_ITEM(lz->iteratortuple, lz->active, NULL);
            lz->active = 0;
        // Otherwise move each item in the tuple (after the empty iterator) one
        // to the left.
        } else {
            PYUI_TupleRemove(lz->iteratortuple, lz->active, lz->numactive);
        }
        lz->numactive--;
        Py_DECREF(iterator);

        // End the loop as soon as no active iterators are avaiable or use the
        // next iterator.
        if (lz->numactive == 0) {
            break;
        } else {
            iterator = PyTuple_GET_ITEM(lz->iteratortuple, lz->active);
        }
    }

    if (lz->numactive == 0) {
        return NULL;
    }

    // Increment the active pointer (potentially wrapping it around to 0).
    lz->active = (lz->active + 1) % (lz->numactive);
    return item;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * roundrobin_reduce(PyIUObject_Roundrobin *lz) {
    PyObject *iteratortuple, *temp, *res;
    Py_ssize_t idx;

    if (PyTuple_Size(lz->iteratortuple) != lz->numactive) {
        iteratortuple = PyTuple_New(lz->numactive);
        if (iteratortuple == NULL) {
            return NULL;
        }
        for (idx=0 ; idx<lz->numactive ; idx++) {
            temp = PyTuple_GET_ITEM(lz->iteratortuple, idx);
            Py_INCREF(temp);
            PyTuple_SET_ITEM(iteratortuple, idx, temp);
        }
    } else {
        iteratortuple = lz->iteratortuple;
        Py_INCREF(iteratortuple);
    }
    res = Py_BuildValue("OO(nn)", Py_TYPE(lz),
                        iteratortuple,
                        lz->numactive, lz->active);
    Py_DECREF(iteratortuple);
    return res;
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * roundrobin_setstate(PyIUObject_Roundrobin *lz,
                                      PyObject *state) {
    Py_ssize_t numactive, active;

    if (!PyArg_ParseTuple(state, "nn", &numactive, &active)) {
        return NULL;
    }

    lz->numactive = numactive;
    lz->active = active;
    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef roundrobin_methods[] = {
    {"__reduce__", (PyCFunction)roundrobin_reduce, METH_NOARGS, ""},
    {"__setstate__", (PyCFunction)roundrobin_setstate, METH_O, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(roundrobin_doc, "roundrobin(*iterables)\n\
\n\
Round-Robin implementation ([0]_).\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    `Iterables` to combine using the round-robin. Any amount of iterables\n\
    are supported.\n\
\n\
Returns\n\
-------\n\
roundrobin : generator\n\
    Iterable filled with the values of the `iterables`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import roundrobin\n\
>>> list(roundrobin('ABC', 'D', 'EF'))\n\
['A', 'D', 'E', 'B', 'F', 'C']\n\
\n\
References\n\
----------\n\
.. [0] https://en.wikipedia.org/wiki/Round-robin_scheduling");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Roundrobin = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.roundrobin",   /* tp_name */
    sizeof(PyIUObject_Roundrobin),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)roundrobin_dealloc,     /* tp_dealloc */
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
    roundrobin_doc,                     /* tp_doc */
    (traverseproc)roundrobin_traverse,  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)roundrobin_next,      /* tp_iternext */
    roundrobin_methods,                 /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    roundrobin_new,                     /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};