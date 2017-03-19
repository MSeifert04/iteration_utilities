/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Packed;

PyTypeObject PyIUType_Packed;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
packed_new(PyTypeObject *type,
           PyObject *args,
           PyObject *kwargs)
{
    PyIUObject_Packed *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "packed", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Packed *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
packed_dealloc(PyIUObject_Packed *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
packed_traverse(PyIUObject_Packed *self,
                visitproc visit,
                void *arg)
{
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
packed_call(PyIUObject_Packed *self,
            PyObject *args,
            PyObject *kwargs)
{
    PyObject *packed, *newpacked, *res;
    if (!PyArg_UnpackTuple(args, "packed.__call__", 1, 1, &packed)) {
        return NULL;
    }

    Py_INCREF(packed);

    if (!PyTuple_CheckExact(packed)) {
        newpacked = PySequence_Tuple(packed);
        Py_DECREF(packed);
        if (newpacked == NULL) {
            return NULL;
        }
        packed = newpacked;
    }

    res = PyObject_Call(self->func, packed, NULL);
    Py_DECREF(packed);
    return res;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
packed_reduce(PyIUObject_Packed *self,
              PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->func);
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef packed_methods[] = {
    {"__reduce__", (PyCFunction)packed_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(packed_doc, "packed(func)\n\
--\n\
\n\
Class that always returns ``func(*x)`` when called with ``packed(func)(x)``.\n\
\n\
.. versionadded:: 0.3\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The function that should be called when the packed-instance is called.\n\
    Positional-only parameter.\n\
\n\
Methods\n\
-------\n\
__call__(x)\n\
    Returns `func(\\*x)`.\n\
\n\
Examples\n\
--------\n\
Creating `packed` instances::\n\
\n\
    >>> from iteration_utilities import packed\n\
    >>> from operator import eq\n\
    >>> five = packed(eq)\n\
    >>> five((2, 2))\n\
    True\n\
\n\
This is a convenience class that emulates the behaviour of \n\
:py:func:`itertools.starmap` (compared to :py:func:`map`)::\n\
\n\
    >>> from itertools import starmap\n\
    >>> list(map(packed(eq), [(2, 2), (3, 3), (2, 3)]))\n\
    [True, True, False]\n\
    >>> list(starmap(eq, [(2, 2), (3, 3), (2, 3)]))\n\
    [True, True, False]\n\
\n\
and :py:func:`iteration_utilities.starfilter` (compared to :py:func:`filter`)::\n\
\n\
    >>> from iteration_utilities import starfilter\n\
    >>> list(filter(packed(eq), [(2, 2), (3, 3), (2, 3)]))\n\
    [(2, 2), (3, 3)]\n\
    >>> list(starfilter(eq, [(2, 2), (3, 3), (2, 3)]))\n\
    [(2, 2), (3, 3)]\n\
\n\
Of course in these cases the appropriate `star`-function can be used but \n\
in case a function does not have such a convenience function already `packed`\n\
can be used.\n\
");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Packed = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.packed",                       /* tp_name */
    sizeof(PyIUObject_Packed),                          /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)packed_dealloc,                         /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    (ternaryfunc)packed_call,                           /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    packed_doc,                                         /* tp_doc */
    (traverseproc)packed_traverse,                      /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    packed_methods,                                     /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    packed_new,                                         /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
