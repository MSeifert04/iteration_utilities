/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *iterator;
} PyIUObject_Starfilter;

PyTypeObject PyIUType_Starfilter;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
starfilter_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"pred", "iterable", NULL};
    PyIUObject_Starfilter *self;

    PyObject *iterable;
    PyObject *func;
    PyObject *iterator=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:starfilter", kwlist,
                                     &func, &iterable)) {
        goto Fail;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    self = (PyIUObject_Starfilter *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(func);
    self->iterator = iterator;
    self->func = func;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
starfilter_dealloc(PyIUObject_Starfilter *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
starfilter_traverse(PyIUObject_Starfilter *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
starfilter_next(PyIUObject_Starfilter *self)
{
    PyObject *item, *val, *newargs;
    PyObject *(*iternext)(PyObject *);
    int ok;

    iternext = *Py_TYPE(self->iterator)->tp_iternext;
    while ( (item = iternext(self->iterator)) ) {
        if (!PyTuple_CheckExact(item)) {
            newargs = PySequence_Tuple(item);
            if (newargs == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        } else {
            Py_INCREF(item);
            newargs = item;
        }
        val = PyObject_Call(self->func, newargs, NULL);
        Py_DECREF(newargs);
        if (val == NULL) {
            Py_DECREF(item);
            return NULL;
        }
        ok = PyObject_IsTrue(val);
        Py_DECREF(val);

        if (ok > 0) {
            return item;
        }
        Py_DECREF(item);
        if (ok < 0) {
            return NULL;
        }
    }
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
starfilter_reduce(PyIUObject_Starfilter *self)
{
    return Py_BuildValue("O(OO)", Py_TYPE(self), self->func, self->iterator);
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef starfilter_methods[] = {
    {"__reduce__",  (PyCFunction)starfilter_reduce,  METH_NOARGS,
     PYIU_reduce_doc},
    {NULL, NULL}
};

#define OFF(x) offsetof(PyIUObject_Starfilter, x)
static PyMemberDef starfilter_memberlist[] = {
    {"pred",  T_OBJECT,  OFF(func),  READONLY,
     "(callable) The function by which to filter (readonly)."},
    {NULL}  /* Sentinel */
};
#undef OFF

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(starfilter_doc, "starfilter(pred, iterable)\n\
--\n\
\n\
Like :py:func:`filter` but unpacks the current item in `iterable` when \n\
calling `pred`. This is similar to the difference between :py:func:`map` and \n\
:py:func:`itertools.starmap`.\n\
\n\
.. versionadded:: 0.3\n\
\n\
Parameters\n\
----------\n\
pred : callable\n\
    The predicate function that is called to determine if the items should\n\
    be kept.\n\
\n\
    .. note::\n\
       Unlike :py:func:`filter` the `pred` cannot be ``None``.\n\
\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
Returns\n\
-------\n\
iterator : generator\n\
    A normal iterator over `iterable` containing only the items where \n\
    ``pred(*item)`` is ``True``.\n\
\n\
Notes\n\
-----\n\
This is identical to ``filter(lambda x: pred(*x), iterable)`` but faster.\n\
\n\
Examples\n\
--------\n\
A simple example::\n\
\n\
    >>> from iteration_utilities import starfilter\n\
    >>> from operator import eq\n\
    >>> list(starfilter(eq, zip([1,2,3], [2,2,2])))\n\
    [(2, 2)]\n\
\n\
See also\n\
--------\n\
filter\n\
iteration_utilities.packed\n\
");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Starfilter = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.starfilter",                   /* tp_name */
    sizeof(PyIUObject_Starfilter),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)starfilter_dealloc,                     /* tp_dealloc */
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
    starfilter_doc,                                     /* tp_doc */
    (traverseproc)starfilter_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)starfilter_next,                      /* tp_iternext */
    starfilter_methods,                                 /* tp_methods */
    starfilter_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    starfilter_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};