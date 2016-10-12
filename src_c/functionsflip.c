typedef struct {
    PyObject_HEAD
    PyObject *func;
} functions_flip_object;

static PyTypeObject functions_flip_type;


static PyObject *
functions_flip_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"func", NULL};

    PyObject *func;

    functions_flip_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:flip", kwargs,
                                     &func)) {
        return NULL;
    }

    lz = (functions_flip_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }

    Py_INCREF(func);
    lz->func = func;

    return (PyObject *)lz;
}


static void
functions_flip_dealloc(functions_flip_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_TYPE(lz)->tp_free(lz);
}


static int
functions_flip_traverse(functions_flip_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->func);
    return 0;
}


static PyObject *
functions_flip_call(functions_flip_object *lz, PyObject *args, PyObject *kw)
{
    Py_ssize_t numargs = PyTuple_Size(args);
    PyObject *result;

    if (numargs != 0) {
        PyObject *newargs = PyTuple_New(numargs);

        Py_ssize_t idx;
        PyObject *item;

        for (idx=0 ; idx < numargs ; idx++) {
            item = PyTuple_GET_ITEM(args, idx);
            Py_INCREF(item);
            PyTuple_SET_ITEM(newargs, numargs - idx - 1, item);
        }
        result = PyObject_Call(lz->func, newargs, kw);
        Py_DECREF(newargs);

    } else {
        result = PyObject_Call(lz->func, args, kw);
    }

    return result;
}


static PyObject *
functions_flip_reduce(functions_flip_object *lz, PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(lz),
                         lz->func);
}

static PyMethodDef functions_flip_methods[] = {
    {"__reduce__", (PyCFunction)functions_flip_reduce, METH_NOARGS, ""},

    {NULL,              NULL}
};


PyDoc_STRVAR(functions_flip_doc,
"flip(x)\n\
\n\
Class that reverses the positional arguments to a `func` when called.\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The function that should be called with the flipped (reversed) arguments.\n\
\n\
Methods\n\
-------\n\
__call__(\\*args, \\*\\*kwargs)\n\
    Returns ``func(*tuple(reversed(args)), **kwargs)``.\n\
\n\
Examples\n\
--------\n\
This can be used to alter the call to a function::\n\
\n\
    >>> from iteration_utilities import flip\n\
    >>> from functools import partial\n\
    >>> flipped = flip(isinstance)\n\
    >>> isfloat = partial(flipped, float)\n\
\n\
    >>> isfloat(10)\n\
    False\n\
    >>> isfloat(11.25)\n\
    True\n\
");


static PyTypeObject functions_flip_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.flip",         /* tp_name */
    sizeof(functions_flip_object),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)functions_flip_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)functions_flip_call,   /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    functions_flip_doc,                 /* tp_doc */
    (traverseproc)functions_flip_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    functions_flip_methods,             /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    functions_flip_new,                 /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
