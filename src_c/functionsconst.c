typedef struct {
    PyObject_HEAD
    PyObject *item;
} functions_constant_object;

static PyTypeObject functions_constant_type;


static PyObject *
functions_constant_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"x", NULL};

    PyObject *item;

    functions_constant_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:constant", kwargs,
                                     &item)) {
        return NULL;
    }

    lz = (functions_constant_object *)type->tp_alloc(type, 0);
    if (lz == NULL)
        return NULL;

    Py_INCREF(item);
    lz->item = item;

    return (PyObject *)lz;
}


static void
functions_constant_dealloc(functions_constant_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->item);
    Py_TYPE(lz)->tp_free(lz);
}


static int
functions_constant_traverse(functions_constant_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->item);
    return 0;
}


static PyObject *
functions_constant_call(functions_constant_object *lz, PyObject *args, PyObject *kw)
{
    Py_INCREF(lz->item);
    return lz->item;
}


static PyObject *
functions_constant_reduce(functions_constant_object *lz, PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(lz),
                         lz->item);
}

static PyMethodDef functions_constant_methods[] = {
    {"__reduce__",
     (PyCFunction)functions_constant_reduce,
     METH_NOARGS,
     ""},

    {NULL,              NULL}
};


PyDoc_STRVAR(functions_constant_doc,
"constant(x)\n\
\n\
Class that always returns `x` when called.\n\
\n\
Parameters\n\
----------\n\
x : any type\n\
    The item that should be returned when called.\n\
\n\
Methods\n\
-------\n\
__call__(\\*args, \\*\\*kwargs)\n\
    Returns `x`.\n\
\n\
Examples\n\
--------\n\
Creating `const` instances::\n\
\n\
    >>> from iteration_utilities import constant\n\
    >>> five = constant(5)\n\
    >>> five()\n\
    5\n\
    >>> ten = constant(10)\n\
    >>> # Any parameters are ignored\n\
    >>> ten(5, give_me=100)\n\
    10\n\
\n\
There are already three predefined instances:\n\
\n\
- ``return_True`` : always returns `True`.\n\
- ``return_False`` : always returns `False`.\n\
- ``return_None`` : always returns `None`.\n\
\n\
For example::\n\
\n\
    >>> from iteration_utilities import return_True, return_False, return_None\n\
    >>> return_True()\n\
    True\n\
    >>> return_False()\n\
    False\n\
    >>> return_None()\n\
    >>> return_None() is None\n\
    True\n\
");


static PyTypeObject functions_constant_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.constant",     /* tp_name */
    sizeof(functions_constant_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)functions_constant_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)functions_constant_call, /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    functions_constant_doc,             /* tp_doc */
    (traverseproc)functions_constant_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    functions_constant_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    functions_constant_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
