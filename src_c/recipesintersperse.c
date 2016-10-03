typedef struct {
    PyObject_HEAD
    PyObject *it;
    PyObject *filler;
    PyObject *nextitem;
    int started;
} recipes_intersperse_object;

static PyTypeObject recipes_intersperse_type;

static PyObject *
recipes_intersperse_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "e", NULL};

    PyObject *iterable, *filler;
    PyObject *it;

    recipes_intersperse_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO:intersperse",
                                     kwargs, &iterable, &filler)) {
        return NULL;
    }

    /* Get iterator. */
    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    /* create intersperse structure */
    lz = (recipes_intersperse_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    Py_XINCREF(filler);
    lz->it = it;
    lz->filler = filler;
    lz->nextitem = NULL;
    lz->started = 0;

    return (PyObject *)lz;
}

static void
recipes_intersperse_dealloc(recipes_intersperse_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->filler);
    Py_XDECREF(lz->nextitem);
    Py_TYPE(lz)->tp_free(lz);
}

static int
recipes_intersperse_traverse(recipes_intersperse_object *lz, visitproc visit,
                            void *arg)
{
    Py_VISIT(lz->it);
    Py_VISIT(lz->filler);
    Py_VISIT(lz->nextitem);
    return 0;
}

static PyObject *
recipes_intersperse_next(recipes_intersperse_object *lz)
{
    PyObject *it = lz->it;
    PyObject *filler = lz->filler;
    PyObject *nextitem = lz->nextitem;

    PyObject *item;

    if (nextitem == NULL) {
        item = PyIter_Next(it);
        if (item == NULL) {
            return NULL;
        }
        if (lz->started == 0) {
            lz->started = 1;
            return item;
        }
        lz->nextitem = item;
        Py_INCREF(filler);
        return filler;
    } else {
        item = nextitem;
        lz->nextitem = NULL;
        return item;
    }
}

static PyObject *
recipes_intersperse_reduce(recipes_intersperse_object *lz) {
    PyObject *value;
    if (lz->nextitem == NULL) {
        value = Py_BuildValue("O(OO)(i)", Py_TYPE(lz),
                              lz->it,
                              lz->filler,
                              lz->started);
    } else {
        value = Py_BuildValue("O(OO)(Oi)", Py_TYPE(lz),
                              lz->it,
                              lz->filler,
                              lz->nextitem,
                              lz->started);
    }
    return value;
}

static PyObject *
recipes_intersperse_setstate(recipes_intersperse_object *lz, PyObject *state)
{
    int started;
    PyObject *nextitem;

    if (PyTuple_Size(state) == 1) {
        if (!PyArg_ParseTuple(state, "i", &started)) {
            return NULL;
        }
        lz->started = started;
    } else {
        if (!PyArg_ParseTuple(state, "Oi", &nextitem, &started)) {
            return NULL;
        }
        lz->started = started;

        Py_CLEAR(lz->nextitem);
        lz->nextitem = nextitem;
        Py_INCREF(lz->nextitem);
    }

    Py_RETURN_NONE;
}

static PyMethodDef recipes_intersperse_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_intersperse_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_intersperse_setstate,
     METH_O,
     ""},

    {NULL,              NULL}   /* sentinel */
};

PyDoc_STRVAR(recipes_intersperse_doc,
"intersperse(iterable, e)\n\
\n\
Alternately yield an item from the `iterable` and `e`. Recipe based on the\n\
homonymous function in the `more-itertools` package ([0]_) but significantly\n\
modified.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The iterable to intersperse.\n\
\n\
e : any type\n\
    The value with which to intersperse the `iterable`.\n\
\n\
Returns\n\
-------\n\
interspersed : generator\n\
    Interspersed `iterable` as generator.\n\
\n\
Notes\n\
-----\n\
This is similar to\n\
``itertools.chain.from_iterable(zip(iterable, itertools.repeat(e)))`` except\n\
that `intersperse` does not yield `e` as last item.\n\
\n\
Examples\n\
--------\n\
A few simple examples::\n\
\n\
    >>> from iteration_utilities import intersperse\n\
    >>> list(intersperse([1,2,3], 0))\n\
    [1, 0, 2, 0, 3]\n\
\n\
    >>> list(intersperse('abc', 'x'))\n\
    ['a', 'x', 'b', 'x', 'c']\n\
\n\
References\n\
----------\n\
.. [0] https://github.com/erikrose/more-itertools\n\
");

static PyTypeObject recipes_intersperse_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.intersperse",  /* tp_name */
    sizeof(recipes_intersperse_object), /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_intersperse_dealloc, /* tp_dealloc */
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
    recipes_intersperse_doc,            /* tp_doc */
    (traverseproc)recipes_intersperse_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_intersperse_next, /* tp_iternext */
    recipes_intersperse_methods,        /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_intersperse_new,            /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
