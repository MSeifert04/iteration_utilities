/******************************************************************************
 * Functions to modify other functions
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} functions_complement_object;

static PyTypeObject functions_complement_type;


static PyObject *
functions_complement_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"func", NULL};

    PyObject *func;

    functions_complement_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:complement", kwargs,
                                     &func)) {
        return NULL;
    }

    lz = (functions_complement_object *)type->tp_alloc(type, 0);
    if (lz == NULL)
        return NULL;

    Py_INCREF(func);

    lz->func = func;

    return (PyObject *)lz;
}


static void
functions_complement_dealloc(functions_complement_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_TYPE(lz)->tp_free(lz);
}


static int
functions_complement_traverse(functions_complement_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->func);
    return 0;
}


static PyObject *
functions_complement_call(functions_complement_object *lz, PyObject *args, PyObject *kw)
{
    PyObject *temp;
    int res;

    temp = PyObject_Call(lz->func, args, kw);
    res = PyObject_Not(temp);
    Py_DECREF(temp);

    if (res < 0) {
        return NULL;
    } else if (res == 0) {
        Py_RETURN_FALSE;
    } else {
        Py_RETURN_TRUE;
    }
}


static PyObject *
functions_complement_reduce(functions_complement_object *lz, PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(lz),
                         lz->func);
}

static PyMethodDef functions_complement_methods[] = {
    {"__reduce__",
     (PyCFunction)functions_complement_reduce,
     METH_NOARGS,
     ""},

    {NULL,              NULL}
};


PyDoc_STRVAR(functions_complement_doc,
"complement(func)\n\
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
.. [0] https://toolz.readthedocs.io/en/latest/index.html\n\
");


static PyTypeObject functions_complement_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.complement",   /* tp_name */
    sizeof(functions_complement_object), /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)functions_complement_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)functions_complement_call, /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    functions_complement_doc,           /* tp_doc */
    (traverseproc)functions_complement_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    functions_complement_methods,       /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    functions_complement_new,           /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
