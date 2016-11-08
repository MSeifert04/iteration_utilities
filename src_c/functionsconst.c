/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *item;
} PyIUObject_Constant;

static PyTypeObject PyIUType_Constant;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * constant_new(PyTypeObject *type, PyObject *args,
                               PyObject *kwargs) {
    PyIUObject_Constant *lz;

    PyObject *item;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "constant", 1, 1, &item)) {
        return NULL;
    }

    /* Create struct */
    lz = (PyIUObject_Constant *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }
    Py_INCREF(item);
    lz->item = item;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void constant_dealloc(PyIUObject_Constant *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->item);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int constant_traverse(PyIUObject_Constant *lz, visitproc visit,
                             void *arg) {
    Py_VISIT(lz->item);
    return 0;
}

/******************************************************************************
 *
 * Call
 *
 *****************************************************************************/

static PyObject * constant_call(PyIUObject_Constant *lz, PyObject *args,
                                PyObject *kwargs) {
    Py_INCREF(lz->item);
    return lz->item;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * constant_reduce(PyIUObject_Constant *lz, PyObject *unused) {
    return Py_BuildValue("O(O)", Py_TYPE(lz), lz->item);
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef constant_methods[] = {
    {"__reduce__", (PyCFunction)constant_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(constant_doc, "constant(x)\n\
--\n\
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
    True");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Constant = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.constant",     /* tp_name */
    sizeof(PyIUObject_Constant),        /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)constant_dealloc,       /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)constant_call,         /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    constant_doc,                       /* tp_doc */
    (traverseproc)constant_traverse,    /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    constant_methods,                   /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    constant_new,                       /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
