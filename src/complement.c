/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Complement;

PyTypeObject PyIUType_Complement;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
complement_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    PyIUObject_Complement *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "complement", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Complement *)type->tp_alloc(type, 0);
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
complement_dealloc(PyIUObject_Complement *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
complement_traverse(PyIUObject_Complement *self,
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
complement_call(PyIUObject_Complement *self,
                PyObject *args,
                PyObject *kwargs)
{
    PyObject *temp;
    int res;

    /* "not func(*args, **kwargs)" */
    temp = PyObject_Call(self->func, args, kwargs);
    if (temp == NULL) {
        return NULL;
    }
    res = PyObject_Not(temp);
    Py_DECREF(temp);

    if (res == 1) {
        Py_RETURN_TRUE;
    } else if (res == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
complement_repr(PyIUObject_Complement *self)
{
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->func);

    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
complement_reduce(PyIUObject_Complement *self,
                  PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->func);
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef complement_methods[] = {
    {"__reduce__", (PyCFunction)complement_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(complement_doc, "complement(func)\n\
--\n\
\n\
Invert a predicate function. homonymous function in the `toolz` package \n\
([0]_) but significantly modified.\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The function to complement.\n\
\n\
Returns\n\
-------\n\
complemented_func : callable\n\
    The complement to `func`.\n\
\n\
Examples\n\
--------\n\
`complement` is equivalent to ``lambda x: not x()`` but significantly faster::\n\
\n\
    >>> from iteration_utilities import complement\n\
    >>> from iteration_utilities import is_None\n\
    >>> is_not_None = complement(is_None)\n\
    >>> list(filter(is_not_None, [1,2,None,3,4,None]))\n\
    [1, 2, 3, 4]\n\
\n\
.. note::\n\
    The same could be done with `itertools.filterfalse` or \n\
    `iteration_utilities.is_not_None`.\n\
\n\
References\n\
----------\n\
.. [0] https://toolz.readthedocs.io/en/latest/index.html");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Complement = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.complement",                   /* tp_name */
    sizeof(PyIUObject_Complement),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)complement_dealloc,                     /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)complement_repr,                          /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    (ternaryfunc)complement_call,                       /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    complement_doc,                                     /* tp_doc */
    (traverseproc)complement_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    complement_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    complement_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
