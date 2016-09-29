typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *value;
} recipes_applyfunc_object;

static PyTypeObject recipes_applyfunc_type;

static PyObject *
recipes_applyfunc_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"func", "initial", NULL};
    PyObject *func=NULL, *initial=NULL;
    recipes_applyfunc_object *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO:applyfunc", kwargs,
                                     &func, &initial)) {
        return NULL;
    }

    /* create applyfunc structure */
    lz = (recipes_applyfunc_object *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }

    Py_INCREF(func);
    Py_INCREF(initial);
    lz->func = func;
    lz->value = initial;

    return (PyObject *)lz;
}

static void
recipes_applyfunc_dealloc(recipes_applyfunc_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_XDECREF(lz->value);
    Py_TYPE(lz)->tp_free(lz);
}

static int
recipes_applyfunc_traverse(recipes_applyfunc_object *lz, visitproc visit,
                            void *arg)
{
    Py_VISIT(lz->value);
    Py_VISIT(lz->func);
    return 0;
}

static PyObject *
recipes_applyfunc_next(recipes_applyfunc_object *lz)
{
    PyObject *value = lz->value;
    PyObject *func = lz->func;
    PyObject *temp;

    for (;;) {
        temp = PyObject_CallFunctionObjArgs(func, value, NULL);
        Py_DECREF(value);
        if (temp == NULL) {
            return NULL;
        }
        Py_INCREF(temp);
        lz->value = temp;
        return temp;
    }
}

PyDoc_STRVAR(recipes_applyfunc_doc,
"applyfunc(func, initial)\n\
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
    >>> from iteration_utilities import take, applyfunc\n\
    >>> import math\n\
    >>> take(applyfunc(math.sqrt, 10), 4)\n\
    [3.1622776601683795, 1.7782794100389228, 1.333521432163324, 1.1547819846894583]\n\
\n\
.. warning::\n\
    This will return an infinitly long generator so do **not** try to do\n\
    something like ``list(applyfunc())``!\n\
");

static PyTypeObject recipes_applyfunc_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.applyfunc", /* tp_name */
    sizeof(recipes_applyfunc_object),  /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_applyfunc_dealloc, /* tp_dealloc */
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
    recipes_applyfunc_doc,             /* tp_doc */
    (traverseproc)recipes_applyfunc_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_applyfunc_next, /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_applyfunc_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
