/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *cnt;
    PyObject *funcargs;
} PyIUObject_Tabulate;

PyTypeObject PyIUType_Tabulate;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
tabulate_new(PyTypeObject *type,
             PyObject *args,
             PyObject *kwargs)
{
    static char *kwlist[] = {"func", "start", NULL};
    PyIUObject_Tabulate *self;

    PyObject *func, *cnt=NULL, *funcargs=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:tabulate", kwlist,
                                     &func, &cnt)) {
        goto Fail;
    }
    if (cnt == NULL) {
        cnt = PyIU_global_zero;
    }
    Py_INCREF(cnt);

    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        goto Fail;
    }

    /* Create and fill struct */
    self = (PyIUObject_Tabulate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(func);
    self->func = func;
    self->cnt = cnt;
    self->funcargs = funcargs;

    return (PyObject *)self;

Fail:
    Py_XDECREF(cnt);
    Py_XDECREF(funcargs);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
tabulate_dealloc(PyIUObject_Tabulate *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->cnt);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
tabulate_traverse(PyIUObject_Tabulate *self,
                  visitproc visit,
                  void *arg)
{
    Py_VISIT(self->func);
    Py_VISIT(self->cnt);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
tabulate_next(PyIUObject_Tabulate *self)
{
    PyObject *result=NULL, *tmp=NULL;
    if (self->cnt == NULL) {
        goto Fail;
    }
    /* Call the function with the current value as argument. */
    PYIU_RECYCLE_ARG_TUPLE(self->funcargs, self->cnt, return NULL);
    result = PyObject_Call(self->func, self->funcargs, NULL);
    if (result == NULL) {
        goto Fail;
    }
    /* Increment the counter. */
    tmp = self->cnt;
    self->cnt = PyNumber_Add(self->cnt, PyIU_global_one);
    Py_DECREF(tmp);
    if (self->cnt == NULL) {
        goto Fail;
    }
    /* Return the result. */
    return result;

Fail:
    Py_XDECREF(result);
    Py_XDECREF(self->cnt);
    /* Reset counter to NULL, so subsequent iterations yield StopIterations. */
    self->cnt = NULL;
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
tabulate_reduce(PyIUObject_Tabulate *self)
{
    return Py_BuildValue("O(OO)",
                         Py_TYPE(self),
                         self->func,
                         self->cnt);
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef tabulate_methods[] = {
    {"__reduce__",  (PyCFunction)tabulate_reduce,  METH_NOARGS,
     PYIU_reduce_doc},
    {NULL, NULL}
};

#define OFF(x) offsetof(PyIUObject_Tabulate, x)
static PyMemberDef tabulate_memberlist[] = {
    {"func",     T_OBJECT,  OFF(func),  READONLY,
     "(callable) The function to tabulate (readonly)."},
    {"current",  T_OBJECT,  OFF(cnt),  READONLY,
     "(any type) The current value to tabulate (readonly)."},
    {NULL}  /* Sentinel */
};
#undef OFF

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(tabulate_doc, "tabulate(func, start=0)\n\
--\n\
\n\
Return ``function(0)``, ``function(1)``, ...\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The `function` to apply.\n\
\n\
start : any type, optional\n\
    The starting value to apply the `function` on. Each time `tabulate` is\n\
    called this value will be incremented by one.\n\
    Default is ``0``.\n\
\n\
Returns\n\
-------\n\
tabulated : generator\n\
    An infinite generator containing the results of the `function` applied\n\
    on the values beginning by `start`.\n\
\n\
Examples\n\
--------\n\
Since the return is an infinite generator you need some other function\n\
to extract only the needed values. For example\n\
:py:func:`~iteration_utilities.getitem`::\n\
\n\
    >>> from iteration_utilities import tabulate, getitem\n\
    >>> from math import sqrt\n\
    >>> t = tabulate(sqrt, 0)\n\
    >>> list(getitem(t, stop=3))\n\
    [0.0, 1.0, 1.4142135623730951]\n\
\n\
.. warning::\n\
    This will return an infinitly long generator so do **not** try to do\n\
    something like ``list(tabulate())``!\n\
\n\
This is equivalent to:\n\
\n\
.. code::\n\
\n\
    import itertools\n\
    \n\
    def tabulate(function, start=0)\n\
        return map(function, itertools.count(start))");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Tabulate = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.tabulate",                     /* tp_name */
    sizeof(PyIUObject_Tabulate),                        /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)tabulate_dealloc,                       /* tp_dealloc */
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
    tabulate_doc,                                       /* tp_doc */
    (traverseproc)tabulate_traverse,                    /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)tabulate_next,                        /* tp_iternext */
    tabulate_methods,                                   /* tp_methods */
    tabulate_memberlist,                                /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    tabulate_new,                                       /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
