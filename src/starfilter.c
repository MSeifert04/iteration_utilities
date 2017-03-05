/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;      /* Function to call */
    PyObject *iterator;  /* iterator over data */
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
    static char *kwlist[] = {"func", "iterable", NULL};
    PyIUObject_Starfilter *self;

    PyObject *iterable;
    PyObject *iterator;
    PyObject *func;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:starfilter", kwlist,
                                     &func, &iterable)) {
        return NULL;
    }

    /* Create and fill struct */
    self = (PyIUObject_Starfilter *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    Py_INCREF(func);
    self->iterator = iterator;
    self->func = func;
    return (PyObject *)self;
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
    while (item = iternext(self->iterator)) {
        if (!PyTuple_CheckExact(item)) {
            newargs = PySequence_Tuple(item);
            Py_DECREF(item);
            if (newargs == NULL) {
                return NULL;
            }
            item = newargs;
        }
        val = PyObject_Call(self->func, item, NULL);
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
    {"__reduce__", (PyCFunction)starfilter_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(starfilter_doc, "starfilter(iterable, func, times=0)\n\
--\n\
\n\
Does a normal iteration over `iterable` and only uses `func` each `times` \n\
items for it's side effects.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
func : callable\n\
    Function that is called for the side effects.\n\
\n\
times : int, optional\n\
    Call the function each `times` items with the last `times` items. \n\
    If ``0`` the argument for `func` will be the item itself. For any \n\
    number greater than zero the argument will be a tuple.\n\
    Default is ``0``.\n\
\n\
Returns\n\
-------\n\
iterator : generator\n\
    A normal iterator over `iterable`.\n\
\n\
Examples\n\
--------\n\
A simple example::\n\
\n\
    >>> from iteration_utilities import starfilter\n\
    >>> def printit(val):\n\
    ...     print(val)\n\
    >>> list(starfilter([1,2,3,4], printit))  # in python3 one could use print directly\n\
    1\n\
    2\n\
    3\n\
    4\n\
    [1, 2, 3, 4]\n\
    >>> list(starfilter([1,2,3,4,5], printit, 2))\n\
    (1, 2)\n\
    (3, 4)\n\
    (5,)\n\
    [1, 2, 3, 4, 5]\n\
\n\
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
    0,                                                  /* tp_members */
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