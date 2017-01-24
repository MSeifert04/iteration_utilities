/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *filler;
    PyObject *nextitem;
    int started;
} PyIUObject_Intersperse;

PyTypeObject PyIUType_Intersperse;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
intersperse_new(PyTypeObject *type,
                PyObject *args,
                PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "e", NULL};
    PyIUObject_Intersperse *self;

    PyObject *iterable, *iterator, *filler;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:intersperse", kwlist,
                                     &iterable, &filler)) {
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    self = (PyIUObject_Intersperse *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    Py_XINCREF(filler);
    self->iterator = iterator;
    self->filler = filler;
    self->nextitem = NULL;
    self->started = 0;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
intersperse_dealloc(PyIUObject_Intersperse *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->filler);
    Py_XDECREF(self->nextitem);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
intersperse_traverse(PyIUObject_Intersperse *self,
                     visitproc visit,
                     void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->filler);
    Py_VISIT(self->nextitem);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
intersperse_next(PyIUObject_Intersperse *self)
{
    PyObject *item;

    if (self->nextitem == NULL) {
        item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator);
        if (item == NULL) {
            PYIU_CLEAR_STOPITERATION;
            return NULL;
        }
        /* If we haven't started we return the first item, otherwise we set
           the nextitem but return the filler.
           */
        if (self->started == 0) {
            self->started = 1;
            return item;
        }
        self->nextitem = item;
        Py_INCREF(self->filler);
        return self->filler;

    /* There was a next item, return it and reset nextitem. */
    } else {
        item = self->nextitem;
        self->nextitem = NULL;
        return item;
    }
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
intersperse_reduce(PyIUObject_Intersperse *self)
{
    PyObject *value;
    if (self->nextitem == NULL) {
        value = Py_BuildValue("O(OO)(i)", Py_TYPE(self),
                              self->iterator,
                              self->filler,
                              self->started);
    } else {
        value = Py_BuildValue("O(OO)(Oi)", Py_TYPE(self),
                              self->iterator,
                              self->filler,
                              self->nextitem,
                              self->started);
    }
    return value;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
intersperse_setstate(PyIUObject_Intersperse *self,
                     PyObject *state)
{
    int started;
    PyObject *nextitem;

    if (PyTuple_Size(state) == 1) {
        if (!PyArg_ParseTuple(state, "i", &started)) {
            return NULL;
        }
        self->started = started;
    } else {
        if (!PyArg_ParseTuple(state, "Oi", &nextitem, &started)) {
            return NULL;
        }
        self->started = started;

        Py_CLEAR(self->nextitem);
        self->nextitem = nextitem;
        Py_INCREF(self->nextitem);
    }

    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
intersperse_lengthhint(PyIUObject_Intersperse *self)
{
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (self->started == 0) {
        if (len == 0) {
            return  PyLong_FromLong(0);
        }
        return PyLong_FromSsize_t(len * 2 - 1);
    } else if (self->nextitem == NULL) {
        return PyLong_FromSsize_t(len * 2);
    } else {
        /* The iterator is always one step advanced! */
        return PyLong_FromSsize_t(len * 2 + 1);
    }
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef intersperse_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)intersperse_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__", (PyCFunction)intersperse_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)intersperse_setstate, METH_O, PYIU_setstate_doc},
    {NULL,              NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(intersperse_doc, "intersperse(iterable, e)\n\
--\n\
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
.. [0] https://github.com/erikrose/more-itertools");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Intersperse = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.intersperse",                  /* tp_name */
    sizeof(PyIUObject_Intersperse),                     /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)intersperse_dealloc,                    /* tp_dealloc */
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
    intersperse_doc,                                    /* tp_doc */
    (traverseproc)intersperse_traverse,                 /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)intersperse_next,                     /* tp_iternext */
    intersperse_methods,                                /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    intersperse_new,                                    /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
