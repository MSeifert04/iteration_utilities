/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Flip;

static PyTypeObject PyIUType_Flip;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * flip_new(PyTypeObject *type, PyObject *args,
                           PyObject *kwargs) {
    PyIUObject_Flip *lz;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "flip", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    lz = (PyIUObject_Flip *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    lz->func = func;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void flip_dealloc(PyIUObject_Flip *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int flip_traverse(PyIUObject_Flip *lz, visitproc visit, void *arg) {
    Py_VISIT(lz->func);
    return 0;
}

/******************************************************************************
 *
 * Call
 *
 *****************************************************************************/

static PyObject * flip_call(PyIUObject_Flip *lz, PyObject *args,
                            PyObject *kwargs) {
    PyObject *result, *newargs;

    if (args != NULL) {
        newargs = PyUI_TupleReverse(args);
        result = PyObject_Call(lz->func, newargs, kwargs);
        Py_DECREF(newargs);
    } else {
        result = PyObject_Call(lz->func, args, kwargs);
    }

    return result;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * flip_reduce(PyIUObject_Flip *lz, PyObject *unused) {
    return Py_BuildValue("O(O)", Py_TYPE(lz),
                         lz->func);
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef flip_methods[] = {
    {"__reduce__", (PyCFunction)flip_reduce, METH_NOARGS, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(flip_doc, "flip(x)\n\
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
    True");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Flip = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.flip",         /* tp_name */
    sizeof(PyIUObject_Flip),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)flip_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)flip_call,   /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    flip_doc,                 /* tp_doc */
    (traverseproc)flip_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    flip_methods,             /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    flip_new,                 /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
