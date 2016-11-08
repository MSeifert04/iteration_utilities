/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *value;
} PyIUObject_Applyfunc;

static PyTypeObject PyIUType_Applyfunc;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * applyfunc_new(PyTypeObject *type, PyObject *args,
                                PyObject *kwargs) {
    static char *kwlist[] = {"func", "initial", NULL};
    PyIUObject_Applyfunc *lz;

    PyObject *func, *initial;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:applyfunc", kwlist,
                                     &func, &initial)) {
        return NULL;
    }

    /* Create and fill struct */
    lz = (PyIUObject_Applyfunc *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    Py_INCREF(initial);
    lz->func = func;
    lz->value = initial;

    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void applyfunc_dealloc(PyIUObject_Applyfunc *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_XDECREF(lz->value);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int applyfunc_traverse(PyIUObject_Applyfunc *lz, visitproc visit,
                              void *arg) {
    Py_VISIT(lz->func);
    Py_VISIT(lz->value);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * applyfunc_next(PyIUObject_Applyfunc *lz) {
    PyObject *newval, *temp;

    // Call the function with the current value as argument
    newval = PyObject_CallFunctionObjArgs(lz->func, lz->value, NULL);
    if (newval == NULL) {
        return NULL;
    }

    // Save the new value and also return it.
    temp = lz->value;
    lz->value = newval;
    Py_DECREF(temp);

    Py_INCREF(newval);
    return newval;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * applyfunc_reduce(PyIUObject_Applyfunc *lz) {
    return Py_BuildValue("O(OO)",
                         Py_TYPE(lz),
                         lz->func,
                         lz->value);
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef applyfunc_methods[] = {
    {"__reduce__", (PyCFunction)applyfunc_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(applyfunc_doc, "applyfunc(func, initial)\n\
--\n\
\n\
Successivly apply `func` on `value`.\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The function to apply. The `value` is given as first argument.\n\
\n\
initial : any type\n\
    The value on which to apply the function.\n\
\n\
Returns\n\
-------\n\
results : generator\n\
    The result of the successivly applied `func`.\n\
\n\
Examples\n\
--------\n\
The first element is the initial `value` and the next elements are\n\
the result of ``func(value)``, then ``func(func(value))``, ...::\n\
\n\
    >>> from iteration_utilities import applyfunc, getitem\n\
    >>> import math\n\
    >>> list(getitem(applyfunc(math.sqrt, 10), stop=4))\n\
    [3.1622776601683795, 1.7782794100389228, 1.333521432163324, 1.1547819846894583]\n\
\n\
.. warning::\n\
    This will return an infinitly long generator so do **not** try to do\n\
    something like ``list(applyfunc())``!");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Applyfunc = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.applyfunc",    /* tp_name */
    sizeof(PyIUObject_Applyfunc),       /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)applyfunc_dealloc,      /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    applyfunc_doc,                      /* tp_doc */
    (traverseproc)applyfunc_traverse,   /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)applyfunc_next,       /* tp_iternext */
    applyfunc_methods,                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    applyfunc_new,                      /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
